// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_GeneratePatrolArea.h"

#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "AIAnimal/TestFood.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryManager.h"

UBTTask_GeneratePatrolArea::UBTTask_GeneratePatrolArea()
{
	NodeName = "GeneratePatrolArea";
}

EBTNodeResult::Type UBTTask_GeneratePatrolArea::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	
	//EQS
	BTComp = &OwnerComp;
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
	 GetWorld(),
	RandomPointQuery,
	AIPawn,
	 EEnvQueryRunMode::RandomBest5Pct,
	 nullptr);
    
	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UBTTask_GeneratePatrolArea::OnFindRandomPointQueryFinished);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;

	
	// float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange");
	//
	// for (auto& PatrolPoint : PatrolPoints)
	// {
	// 	PatrolPoint = GenerateRandomLocation(ActorLocation,WanderRange);
	// }
	//
	// BlackboardComp->SetValueAsBool("NIsNeedToGeneratePP", false);
	// return Super::ExecuteTask(OwnerComp, NodeMemory);
}




void UBTTask_GeneratePatrolArea::OnFindRandomPointQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	//성공하지 않았으면 리턴
	if (EEnvQueryStatus::Success != QueryStatus)
	{
		FinishLatentTask(*BTComp, EBTNodeResult::Failed);
		return;
	}
	AAIController* AIController = BTComp->GetAIOwner();
	TArray<FVector>& PatrolPoints = Cast<ABaseAIAnimal>(AIController->GetPawn())->GetPatrolPoints();

	for (int i=0; i < PatrolPoints.Num(); i++)
	{
		PatrolPoints[i] = QueryInstance->GetQueryResult()->GetItemAsLocation(i);
	}
	
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool("NIsNeedToGeneratePP", false);
		FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
	}
	
	FinishLatentTask(*BTComp, EBTNodeResult::Failed);
}


FVector UBTTask_GeneratePatrolArea::GenerateRandomLocation(const FVector& BaseLocation, const float Range)
{
	int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomX = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomY = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	return BaseLocation + FVector(RandomX, RandomY, 0.f);
}