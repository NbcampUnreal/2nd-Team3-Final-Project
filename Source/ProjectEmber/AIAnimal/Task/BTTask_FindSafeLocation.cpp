// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/Task/BTTask_FindSafeLocation.h"

#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"


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

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	
	FVector ActorLocation = AIPawn->GetActorLocation();
	const EAnimalAIState State = static_cast<EAnimalAIState>(BlackboardComp->GetValueAsEnum("CurrentState"));
	const float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange");
	
	//여기 더 자연스럽게 수정하기
	{
		FRotator Rotator = AIPawn->GetActorRotation();
		Rotator.Yaw *= -1.0f;
		AIPawn->SetActorRotation(Rotator);
		ActorLocation = GenerateRandomLocation(ActorLocation, WanderRange*2.0f);
	}
	
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector("SafeLocation", ActorLocation);
		BlackboardComp->SetValueAsEnum("CurrentState", static_cast<uint8>(EAnimalAIState::Warning));
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_FindSafeLocation::SafeLocation 업데이트 성공. %f, %f, %f"), ActorLocation.X, ActorLocation.Y, ActorLocation.Z );
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

FVector UBTTask_FindSafeLocation::GenerateRandomLocation(const FVector& BaseLocation, float Range)
{
	const int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomX = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	const float RandomY = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	return BaseLocation + FVector(RandomX, RandomY, 0.f);
}
