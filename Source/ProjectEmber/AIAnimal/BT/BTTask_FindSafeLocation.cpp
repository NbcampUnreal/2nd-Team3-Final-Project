// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_FindSafeLocation.h"

#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"


UBTTask_FindSafeLocation::UBTTask_FindSafeLocation()
{
	NodeName = TEXT("FindSafeLocation");
}

EBTNodeResult::Type UBTTask_FindSafeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	// FVector ActorLocation = AIPawn->GetActorLocation();
	//
	// //const float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange"); //임시수정 -> 이동 가능 범위, 무리 구역 범위, 인식 범위 실제 월드에 배치해보고 디테일하게 정해서 수정해야함
	//
	// UObject* TargetObject = BlackboardComp->GetValueAsObject("TargetActor");
	// AActor* TargetActor = Cast<AActor>(TargetObject);
	// FVector TargetActorLocation = TargetActor->GetActorLocation();
	//
	// ActorLocation = GenerateRandomLocation(TargetActorLocation, ActorLocation);
	//
	//
	// if (BlackboardComp)
	// {
	// 	BlackboardComp->SetValueAsVector("SafeLocation", ActorLocation);
	// }
	// return Super::ExecuteTask(OwnerComp, NodeMemory);
//}
	
	//EQS
	BTComp = &OwnerComp;
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
	 GetWorld(),
	SafeLocationQuery,
	AIPawn,
	 EEnvQueryRunMode::RandomBest5Pct,
	 nullptr);
    
	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UBTTask_FindSafeLocation::OnFindSafeLocationQueryFinished);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
	
}

void UBTTask_FindSafeLocation::OnFindSafeLocationQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	//성공하지 않았으면 리턴
	if (EEnvQueryStatus::Success != QueryStatus)
	{
		FinishLatentTask(*BTComp, EBTNodeResult::Failed);
		return;
	}

	TArray<FVector> AllLocations;
	AllLocations.SetNum(5);
	for (int i=0; i < AllLocations.Num(); i++)
	{
		AllLocations[i] = QueryInstance->GetQueryResult()->GetItemAsLocation(i);
	}
	
	if (BlackboardComp)
	{
		int Index = FMath::RandRange(0, 5);
		FVector SafeLocation = AllLocations[Index];
		
		BlackboardComp->SetValueAsVector("SafeLocation", SafeLocation);
	}
	
	FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}

FVector UBTTask_FindSafeLocation::GenerateRandomLocation(const FVector& TargetActorLocation, const FVector& ActorLocation)
{

	FVector DirToThreat = (TargetActorLocation - ActorLocation).GetSafeNormal() * -1.0f; // 타겟 반대로 향하는방향
	FVector RightVector = FVector::CrossProduct(DirToThreat, FVector::UpVector);
	FVector Offset = RightVector * FMath::RandRange(-150.f, 150.f) + DirToThreat * FMath::RandRange(1000.f, 3000.f); // 좌우랜덤, 거리랜덤, 임시수정
	
	return ActorLocation + Offset;
}
