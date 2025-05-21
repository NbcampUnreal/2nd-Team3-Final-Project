// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_GeneratePatrolArea.h"

#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"

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

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	FVector ActorLocation = AIPawn->GetActorLocation();
	TArray<FVector>& PatrolPoints = Cast<ABaseAIAnimal>(AIPawn)->GetPatrolPoints();
	float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange");
	
	for (auto& PatrolPoint : PatrolPoints)
	{
		PatrolPoint = GenerateRandomLocation(ActorLocation,WanderRange * 3.f);
	}
	
	BlackboardComp->SetValueAsBool("NIsNeedToGeneratePP", false);
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}


FVector UBTTask_GeneratePatrolArea::GenerateRandomLocation(const FVector& BaseLocation, const float Range)
{
	int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomX = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomY = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	return BaseLocation + FVector(RandomX, RandomY, 0.f);
}