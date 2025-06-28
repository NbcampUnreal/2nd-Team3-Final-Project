// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_FollowLeader.h"

#include "AIController.h"
#include "GameplayTagAssetInterface.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FollowLeader::UBTTask_FollowLeader()
{
	NodeName = "FollowLeader";
}

EBTNodeResult::Type UBTTask_FollowLeader::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	LeaderOrPatrolQuery,
	AIPawn,
	 EEnvQueryRunMode::SingleResult,
	 nullptr);
    
	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UBTTask_FollowLeader::OnFindLeaderOrPatrolQueryFinished);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
	
}

void UBTTask_FollowLeader::OnFindLeaderOrPatrolQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	//성공하지 않았으면 리턴
	if (EEnvQueryStatus::Success != QueryStatus)
	{
		FinishLatentTask(*BTComp, EBTNodeResult::Failed);
		return;
	}
	
	TArray<AActor*> AllActors;
	QueryInstance->GetQueryResult()->GetAllAsActors(AllActors);
	
	if (AllActors.Num()==0)
	{
		FinishLatentTask(*BTComp, EBTNodeResult::Failed);
		return;
	}
	
	if (BlackboardComp)
	{
		FVector SafeLocation = QueryInstance->GetQueryResult()->GetItemAsLocation(0);
		AActor*  TargetActor = QueryInstance->GetQueryResult()->GetItemAsActor(0);
		BlackboardComp->SetValueAsObject("TargetActor", TargetActor);
		BlackboardComp->SetValueAsVector("SafeLocation", SafeLocation);
	}
	
	FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}

FVector UBTTask_FollowLeader::GenerateRandomLocation(const FVector& BaseLocation, const float Range)
{
	int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomX = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomY = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	return BaseLocation + FVector(RandomX, RandomY, 0.f);
}
