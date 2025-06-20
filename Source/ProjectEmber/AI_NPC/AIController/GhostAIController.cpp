#include "GhostAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AGhostAIController::AGhostAIController()
{
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void AGhostAIController::BeginPlay()
{
    Super::BeginPlay();

    if (BehaviorTreeAsset && BehaviorTreeAsset->BlackboardAsset)
    {
        if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp))
        {
            RunBehaviorTree(BehaviorTreeAsset);

        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Blackboard  (UseBlackboard return false)"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BehaviorTree or BlackboardAsset is not set."));
    }
}