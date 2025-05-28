// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectEmber/AIAnimal/BT/BTTask_FindRandomPoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AIAnimal/TestFood.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryManager.h"

UBTTask_FindRandomPoint::UBTTask_FindRandomPoint()
{
	NodeName = TEXT("FindRandomPoint");
}

EBTNodeResult::Type UBTTask_FindRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UBTTask_FindRandomPoint::OnFindRandomPointQueryFinished);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;

	
}

void UBTTask_FindRandomPoint::OnFindRandomPointQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	//성공하지 않았으면 리턴
	if (EEnvQueryStatus::Success != QueryStatus)
	{
		FinishLatentTask(*BTComp, EBTNodeResult::Failed);
		return;
	}
	
	TArray<FVector> Locations;
	QueryInstance->GetQueryResult()->GetAllAsLocations(Locations);
	
	if (BlackboardComp)
	{
		int Index = FMath::RandRange(0, Locations.Num()-1);
		BlackboardComp->SetValueAsVector("TargetLocation", Locations[Index]);
	}
	
	FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}


FVector UBTTask_FindRandomPoint::GenerateRandomLocation(APawn* AIPawn, float Range)
{
	FVector ActorForwordDir = AIPawn->GetActorForwardVector().GetSafeNormal();
	FVector RightVector = FVector::CrossProduct(ActorForwordDir, FVector::UpVector);
	FVector Offset = RightVector * FMath::RandRange(-150.f, 150.f) + ActorForwordDir * FMath::RandRange(1000.f, 3000.f); // 좌우랜덤, 거리랜덤
	
	return AIPawn->GetActorLocation() + Offset;
}
// FVector ActorLocation = AIPawn->GetActorLocation();
 	// const float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange");
 	//
 	// ActorLocation = GenerateRandomLocation(AIPawn, WanderRange);
 	// if (BlackboardComp)
 	// {
 	// 	BlackboardComp->SetValueAsVector("TargetLocation", ActorLocation);
 	// }
 	// return Super::ExecuteTask(OwnerComp, NodeMemory);