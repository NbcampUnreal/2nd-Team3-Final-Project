// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectEmber/AIAnimal/AIAnimalController.h"

#include "BaseAIAnimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"

const FName AAIAnimalController::SleepTime = "SleepTime";
const FName AAIAnimalController::IsShouldSleep = "IsShouldSleep";
const FName AAIAnimalController::DistanceToTarget = "DistanceToTarget";
const FName AAIAnimalController::CurrentState = "CurrentState";
const FName AAIAnimalController::TargetActor = "TargetActor";
const FName AAIAnimalController::TargetLocation = "TargetLocation";
const FName AAIAnimalController::FleeRange = "FleeRange";
const FName AAIAnimalController::SafeLocation = "SafeLocation";
const FName AAIAnimalController::IsRest = "IsRest";
const FName AAIAnimalController::IsHit = "IsHit";

AAIAnimalController::AAIAnimalController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

    SightConfig->SightRadius = 2000.0f;
    SightConfig->LoseSightRadius = 2300.0f;
    SightConfig->PeripheralVisionAngleDegrees = 120.0f;
    SightConfig->SetMaxAge(3.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    
    HearingConfig->HearingRange = 2000.0f;
    HearingConfig->SetMaxAge(3.0f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;

    PerceptionComp->ConfigureSense(*SightConfig);
    PerceptionComp->ConfigureSense(*HearingConfig);
    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation()); //우선적용
    
}

void AAIAnimalController::BeginPlay()
{
    Super::BeginPlay();
    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AAIAnimalController::OnTargetPerceptionUpdated);
    BlackboardComponent->SetValueAsBool("IsRest", true);
    BlackboardComponent->SetValueAsFloat("SleepTime", fSleepTime);
    BlackboardComponent->SetValueAsBool("IsShouldSleep",bIsShouldSleep);
    BlackboardComponent->SetValueAsEnum("CurrentState",
                                            static_cast<uint8>(Cast<ABaseAIAnimal>(GetPawn())->GetCurrentState()) );
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
        UE_LOG(LogTemp, Warning, TEXT("AnimalController::블랙보드 초기화 성공, 비헤이비어 트리 실행."));
    }

    AbilitySystemComponent = CastChecked<ABaseAIAnimal>(InPawn)->GetAbilitySystemComponent();
    //AbilitySystemComponent->Attribute
}

void AAIAnimalController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    //시각 청각적으로 감지됐을 때
    //일단 멈추고
    // 다시 탐색
    /*
    FindTargetPlayer(Actor, Stimulus);
    */
   // FindTargetAnimal(Actor, Stimulus);
    BBComponent = GetBlackboardComponent();
    if (BBComponent)
    {
        if (const ABaseAIAnimal* TargetAnimal = Cast<ABaseAIAnimal>(Actor))
        {
            const UAbilitySystemComponent* TargetAsc = TargetAnimal->GetAbilitySystemComponent();
            if (!TargetAsc)
            {
                UE_LOG(LogTemp, Warning, TEXT("AnimalController::OnTargetPerceptionUpdated::TargetAsc = nullptr "));
            }
            const UAbilitySystemComponent* SourceAsc = Cast<ABaseAIAnimal>(GetPawn())->GetAbilitySystemComponent();
            if (!SourceAsc)
            {
                UE_LOG(LogTemp, Warning, TEXT("AnimalController::OnTargetPerceptionUpdated::TargetAsc = nullptr "));
            }
            const UEmberAnimalAttributeSet* TargetAttribute = TargetAsc->GetSet<UEmberAnimalAttributeSet>();
            const UEmberAnimalAttributeSet* SourceAttribute = SourceAsc->GetSet<UEmberAnimalAttributeSet>();
            
            //파워 우선순위 판단
            const float PawnWildPower = SourceAttribute->GetWildPower();
            const float TargetWildPower = TargetAttribute->GetWildPower();
            
            if (PawnWildPower <= TargetWildPower) //우선순위가 높거나 같으면
            {
                BBComponent->SetValueAsBool("IsRest", true);
            }
            else
            {
                //여기서 인식되면 타겟, 거리 등록
                BBComponent->SetValueAsBool("IsRest", false);
                BBComponent->SetValueAsEnum("CurrentState", static_cast<uint8>(EAnimalAIState::Warning));
                BBComponent->SetValueAsObject("TargetActor", Actor);
            
            }
        }
    }

}

void AAIAnimalController::FindTargetPlayer(AActor* Actor, FAIStimulus Stimulus)
{
    // 월드에 플레이어가 없다면 터진다 ->당연
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
    if (!PlayerCharacter)
    {
        return;
    }
    BBComponent = GetBlackboardComponent();
    if (BBComponent)
    {
        // 감지된게 플레이어면
        if ( Actor == Cast<AActor>(PlayerCharacter))
        {
            const bool bCanDetect = Stimulus.WasSuccessfullySensed();
            BBComponent->SetValueAsObject("TargetActor", Actor);
            BBComponent->SetValueAsBool("IsRest", !bCanDetect);
        }
    }
}

void AAIAnimalController::FindTargetAnimal(AActor* Actor, FAIStimulus Stimulus)
{

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

