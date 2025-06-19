// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectEmber/AIAnimal/AIAnimalController.h"

#include "BaseAIAnimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"
#include "Character/EmberCharacter.h"

AAIAnimalController::AAIAnimalController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

    SightConfig->SightRadius = 1500.0f;
    SightConfig->LoseSightRadius = 1800.0f;
    SightConfig->PeripheralVisionAngleDegrees = 120.0f;
    SightConfig->SetMaxAge(2.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    
    HearingConfig->HearingRange = 800.0f;
    HearingConfig->SetMaxAge(2.0f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

    PerceptionComp->ConfigureSense(*SightConfig);
    PerceptionComp->ConfigureSense(*HearingConfig);
    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation()); //우선적용
    
}

void AAIAnimalController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    // 비헤이비어 트리 실행
    if (BehaviorTrees[0])
    {
        // 이전까지 멤버변수 BlackboardComponent = nullptr일 가능성 높음
        UseBlackboard(BehaviorTrees[0]->BlackboardAsset, BlackboardComponent); 
        // 블랙보드 초기화 성공, 비헤이비어 트리 실행
        RunBehaviorTree(BehaviorTrees[0]);
        BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
    }
    AbilitySystemComponent = CastChecked<ABaseAIAnimal>(InPawn)->GetAbilitySystemComponent();
}

void AAIAnimalController::BeginPlay()
{
    Super::BeginPlay();
    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AAIAnimalController::OnTargetPerceptionUpdated);
    PerceptionComp->OnTargetPerceptionForgotten.AddDynamic(this, &AAIAnimalController::OnTargetPerceptionForgotten);
}

void AAIAnimalController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    bool IsSleep = Cast<ABaseAIAnimal>(GetPawn())->GetIsShouldSleep();
    bool IsDead = Cast<ABaseAIAnimal>(GetPawn())->GetIsDead();
    if (IsSleep || IsDead)
    {
        return;
    }
    //시각 청각적으로 감지됐을 때 -> 뒤에서 접근하면 감지 못하는 문제 있을 듯 ,근데 동물은 그게 맞아
    FindTargetAnimal(Actor, Stimulus);
    FindTargetPlayer(Actor, Stimulus);
}

void AAIAnimalController::OnTargetPerceptionForgotten(AActor* Actor)
{
    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsObject("TargetActor", nullptr);
        BlackboardComponent->SetValueAsName("NStateTag","Animal.State.Idle");
    }
}

void AAIAnimalController::FindTargetPlayer(AActor* Actor, FAIStimulus Stimulus)
{
    if (BlackboardComponent)
    {
        // 감지된게 플레이어면
        AEmberCharacter* PlayerCharacter = Cast<AEmberCharacter>(Actor);
        if (!PlayerCharacter)
        {
            return;
        }
        
        //--- 공격 확률 설정 --------------------------------------------------
        float HitCount = Cast<ABaseAIAnimal>(GetPawn())->GetHitCount();
        float AttackProb = 0.05;          // 기본 0 %
        if (Cast<ABaseAIAnimal>(GetPawn())->GetPersonality() == EAnimalAIPersonality::Brave) 
        {
            AttackProb += 0.05f;          // 성격이 ‘용감'이라면 +5 %
        }
        if (Cast<ABaseAIAnimal>(GetPawn())->GetRoleTag() == "Animal.Role.Leader")
        {
            AttackProb += 0.5f;            // 역할이 '리더'라면 +50 %
        }
        HitCount *= 0.01f; 
        AttackProb -= HitCount;
        //--------------------------------------------------------------------

        // 난수 뽑아서 결정
        if (AttackProb <= 0.0f)
        {
            AttackProb = 0.0f;
        }
        if (FMath::FRand() <= AttackProb)
        {
            BlackboardComponent->SetValueAsObject("TargetActor", Actor);
            BlackboardComponent->SetValueAsVector("TargetLocation", Actor->GetActorLocation());
            BlackboardComponent->SetValueAsName("NStateTag","Animal.State.Attack");
        }
        else
        {
            BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Warning");
            BlackboardComponent->SetValueAsObject("TargetActor", Actor);
        }
    
    }
}

void AAIAnimalController::FindTargetAnimal(AActor* Actor, FAIStimulus Stimulus)
{
    if (BlackboardComponent)
    {
        // 감지된게 동물이면
        if (const ABaseAIAnimal* TargetAnimal = Cast<ABaseAIAnimal>(Actor))
        {
            //같은 종이면 무시
            if (TargetAnimal->GetIdentityTag() == Cast<ABaseAIAnimal>(GetPawn())->GetIdentityTag())
            {
                return;
            }
            const UAbilitySystemComponent* TargetAsc = TargetAnimal->GetAbilitySystemComponent();
            const UAbilitySystemComponent* SourceAsc = Cast<ABaseAIAnimal>(GetPawn())->GetAbilitySystemComponent();
            const UEmberAnimalAttributeSet* TargetAttribute = TargetAsc->GetSet<UEmberAnimalAttributeSet>();
            const UEmberAnimalAttributeSet* SourceAttribute = SourceAsc->GetSet<UEmberAnimalAttributeSet>();

            //파워 우선순위 판단
            const float PawnWildPower = SourceAttribute->GetWildPower();
            const float TargetWildPower = TargetAttribute->GetWildPower();
            
            if (PawnWildPower <= TargetWildPower) //this가 우선순위가 더 작다면(높다면)
            {
                //--- 공격 확률 설정 --------------------------------------------------
                float AttackProb = 0.45f;          // 기본 5 %
                if (Cast<ABaseAIAnimal>(GetPawn())->GetPersonality() == EAnimalAIPersonality::Brave) // 성격이 ‘용감’이라면 +5 %
                {
                    AttackProb += 0.05f;           // ⇒ 10 %
                }
                //--------------------------------------------------------------------

                // 난수 뽑아서 결정
                if (FMath::FRand() <= AttackProb)
                {
                    BlackboardComponent->SetValueAsObject("TargetActor", Actor);
                    BlackboardComponent->SetValueAsVector("TargetLocation", Actor->GetActorLocation());
                    BlackboardComponent->SetValueAsName("NStateTag","Animal.State.Attack");
                }
                else
                {
                    BlackboardComponent->SetValueAsName("NStateTag","Animal.State.Idle");
                }
            }
            else //this가 우선순위가 더 크다면(낮다면) -> 도망
            {
                //여기서 인식되면 타겟, 거리 등록
                BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Idle");
                BlackboardComponent->SetValueAsObject("TargetActor", Actor);
            }
        }
    }
}

void AAIAnimalController::SenseInteractionWithUI(const FAIStimulus& Stimulus)
{
    //누가 누구에게는 합치고나서 수정
    
    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        //시각적으로 알아챈 ui를 넣는다던가
    }
    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        //청각적으로 알아챈 ui를 넣는다던가
    }
}

void AAIAnimalController::SwitchToBehaviorTree(int32 NewIndex)
{
    if (!BehaviorTrees.IsValidIndex(NewIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid BehaviorTreeComponent index: %d"), NewIndex);
        return;
    }

    // 현재 실행 중인 BT 중지
    if (BehaviorTreeComponent && BehaviorTreeComponent->IsRunning())
    {
        BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
    }
    PerceptionComp->DestroyComponent();
    
    // 새 BTComponent 실행
    UseBlackboard(BehaviorTrees[NewIndex]->BlackboardAsset, BlackboardComponent);
    RunBehaviorTree(BehaviorTrees[NewIndex]);
    
    BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
}