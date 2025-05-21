// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_FindSafeLocation.h"

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
	//const EAnimalAIState State = static_cast<EAnimalAIState>(BlackboardComp->GetValueAsEnum("CurrentState"));
	const float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange");
	
	//여기 더 자연스럽게 수정하기
	{
		//FRotator Rotator = AIPawn->GetActorRotation();
		//Rotator.Yaw *= -1.0f;
		//AIPawn->SetActorRotation(Rotator);
		
		UObject* TargetObject = BlackboardComp->GetValueAsObject("TargetActor");
		AActor* TargetActor = Cast<AActor>(TargetObject);
		FVector TargetActorLocation = TargetActor->GetActorLocation();
		
		ActorLocation = GenerateRandomLocation(TargetActorLocation, ActorLocation);
	}
	
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector("SafeLocation", ActorLocation);
		//BlackboardComp->SetValueAsEnum("CurrentState", static_cast<uint8>(EAnimalAIState::Warning));
		//UE_LOG(LogTemp, Warning, TEXT("UBTTask_FindSafeLocation::SafeLocation 업데이트 성공. %f, %f, %f"), ActorLocation.X, ActorLocation.Y, ActorLocation.Z );
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

FVector UBTTask_FindSafeLocation::GenerateRandomLocation(const FVector& TargetActorLocation, const FVector& ActorLocation)
{

	FVector DirToThreat = (TargetActorLocation - ActorLocation).GetSafeNormal() * -1.0f; // 타겟 반대로 향하는방향
	FVector RightVector = FVector::CrossProduct(DirToThreat, FVector::UpVector);
	FVector Offset = RightVector * FMath::RandRange(-150.f, 150.f) + DirToThreat * FMath::RandRange(1000.f, 3000.f); // 좌우랜덤, 거리랜덤
	
	return ActorLocation + Offset;
}
