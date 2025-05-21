// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_SetPatrolAreaIndex.h"

#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetPatrolAreaIndex::UBTTask_SetPatrolAreaIndex()
{
	NodeName = "SetPatrolAreaIndex";
}

EBTNodeResult::Type UBTTask_SetPatrolAreaIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	
	const TArray<FVector>& PatrolPoints = Cast<ABaseAIAnimal>(AIPawn)->GetPatrolPoints();

	if (PatrolPoints.Num() > 0)
	{
		int32 Index = BlackboardComp->GetValueAsInt("NPatrolIndex");
		Index++;
		if (Index == PatrolPoints.Num())
		{
			BlackboardComp->SetValueAsBool("NIsNeedToGeneratePP", true);
		}
		Index = Index % PatrolPoints.Num();
		BlackboardComp->SetValueAsInt("NPatrolIndex", Index);
	
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}
	return EBTNodeResult::Failed;
}
