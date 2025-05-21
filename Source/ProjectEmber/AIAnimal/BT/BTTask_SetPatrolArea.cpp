// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_SetPatrolArea.h"

#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetPatrolArea::UBTTask_SetPatrolArea()
{
	NodeName = "SetPatrolArea";
}

EBTNodeResult::Type UBTTask_SetPatrolArea::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
		if (!PatrolPoints.IsValidIndex(Index))
		{
			BlackboardComp->SetValueAsInt("NPatrolIndex", 0);
			return EBTNodeResult::Failed;
		}

		BlackboardComp->SetValueAsVector("NPatrolPoint", PatrolPoints[Index]);
	
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}
	return EBTNodeResult::Failed;
}
