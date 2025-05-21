// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectEmber/AIAnimal/BT/BTTask_FindRandomPoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AIAnimal/TestFood.h"
//#include "Interactables/Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

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

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	FVector ActorLocation = AIPawn->GetActorLocation();
	const float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange");
	
	ActorLocation = GenerateRandomLocation(AIPawn, WanderRange);
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector("TargetLocation", ActorLocation);
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
	
}

FVector UBTTask_FindRandomPoint::GenerateRandomLocation(APawn* AIPawn, float Range)
{
	FVector ActorForwordDir = AIPawn->GetActorForwardVector().GetSafeNormal();
	FVector RightVector = FVector::CrossProduct(ActorForwordDir, FVector::UpVector);
	FVector Offset = RightVector * FMath::RandRange(-150.f, 150.f) + ActorForwordDir * FMath::RandRange(1000.f, 3000.f); // 좌우랜덤, 거리랜덤
	
	return AIPawn->GetActorLocation() + Offset;
}