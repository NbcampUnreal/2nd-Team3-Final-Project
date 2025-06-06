// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectEmber/AIAnimal/AIAnimalController.h"

#include "BaseAIAnimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "EMSCompSaveInterface.h"
#include "TestFood.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"
#include "EnvironmentQuery/EnvQueryManager.h"

const FName AAIAnimalController::SleepTime = "SleepTime";
const FName AAIAnimalController::IsShouldSleep = "IsShouldSleep";
const FName AAIAnimalController::DistanceToTarget = "DistanceToTarget";
const FName AAIAnimalController::CurrentState = "CurrentState";
const FName AAIAnimalController::TargetActor = "TargetActor";
const FName AAIAnimalController::TargetLocation = "TargetLocation";
const FName AAIAnimalController::FleeRange = "FleeRange";
const FName AAIAnimalController::WanderRange = "WanderRange"; 
const FName AAIAnimalController::SafeLocation = "SafeLocation"; 
const FName AAIAnimalController::IsHit = "IsHit";

AAIAnimalController::AAIAnimalController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

    SightConfig->SightRadius = 2000.0f;
    SightConfig->LoseSightRadius = 2500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 120.0f;
    SightConfig->SetMaxAge(3.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    
    HearingConfig->HearingRange = 1000.0f;
    HearingConfig->SetMaxAge(3.0f);
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
    if (BehaviorTree)
    {
        // 이전까지 멤버변수 BlackboardComponent = nullptr일 가능성 높음
        UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComponent); 
        // 블랙보드 초기화 성공, 비헤이비어 트리 실행
        RunBehaviorTree(BehaviorTree);
    }
    InitBlackboard();
    //AnimalBindingEQS(InPawn);
    AbilitySystemComponent = CastChecked<ABaseAIAnimal>(InPawn)->GetAbilitySystemComponent();
}

void AAIAnimalController::BeginPlay()
{
    Super::BeginPlay();
    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AAIAnimalController::OnTargetPerceptionUpdated);

}

void AAIAnimalController::InitBlackboard()
{
    GetBlackboardComponent()->SetValueAsName("NStateTag", "Animal.State.Idle");
    GetBlackboardComponent()->SetValueAsBool("NIsNeedToGeneratePP", true);
    GetBlackboardComponent()->SetValueAsFloat("SleepTime", fSleepTime);
    GetBlackboardComponent()->SetValueAsBool("IsShouldSleep",bIsShouldSleep);
    GetBlackboardComponent()->SetValueAsEnum("Personality",
                                            static_cast<uint8>(Cast<ABaseAIAnimal>(GetPawn())->GetPersonality()) );
    GetBlackboardComponent()->SetValueAsFloat("WanderRange",500.0f);
}

void AAIAnimalController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    //시각 청각적으로 감지됐을 때 -> 뒤에서 접근하면 감지 못하는 문제 있을 듯 ,근데 동물은 그게 맞아
    FindTargetPlayer(Actor, Stimulus);
    FindTargetAnimal(Actor, Stimulus);
}

void AAIAnimalController::FindTargetPlayer(AActor* Actor, FAIStimulus Stimulus)
{
    if (BlackboardComponent)
    {
        // 감지된게 플레이어면
        // 월드에 플레이어가 없다면 터진다 ->당연
        ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
        if (!PlayerCharacter)
        {
            return;
        }
        if (BlackboardComponent)
        {
            //--- 공격 확률 설정 --------------------------------------------------
            float AttackProb = 0.95;          // 기본 5 %
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
                BlackboardComponent->SetValueAsName("NEnemyTag", "Player");
                BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Warning");
                BlackboardComponent->SetValueAsObject("TargetActor", Actor);
            }
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
                float AttackProb = 0.95f;          // 기본 5 %
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
                BlackboardComponent->SetValueAsName("NEnemyTag", "Animal");
                //여기서 인식되면 타겟, 거리 등록
                BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Warning");
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