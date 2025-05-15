// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectEmber/AIAnimal/BT/BTTask_FindPoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AIAnimal/TestFood.h"
//#include "Interactables/Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

UBTTask_FindPoint::UBTTask_FindPoint()
{
	NodeName = TEXT("FindPoint");
}

EBTNodeResult::Type UBTTask_FindPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	const bool bIsRest = BlackboardComp->GetValueAsBool("IsRest"); 
	const bool bIsHungry = BlackboardComp->GetValueAsBool("IsHungry");
	const uint8 CurrentState = BlackboardComp->GetValueAsEnum("CurrentState");
	
	
	if (CurrentState == static_cast<uint8>(EAnimalAIState::Idle))
	{
		if (bIsHungry && bIsRest)
		{
			// 상호작용 가능한 가장 가까운 대상 찾기 -> 다른 객체와 타겟 겹치지 않게 처리해야함
			TArray<AActor*> InteractiveObjects;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), InteractiveObjectTag, InteractiveObjects);

			// 가장 가까운 오브젝트 찾기
			AActor* ClosestObject = nullptr;
			float ClosestDistance = SearchRadius;

			for (AActor* Object : InteractiveObjects)
			{
				//다른 객체와 이미 상호작용중이면 다음으로 넘어감
				if (Object->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
				{
					if (Cast<ATestFood>(Object)->GetSelected())
					{
						continue;
					}
				}
				// 해당 AI Controller를 보유한 Actor의 Location과, Interactive를 태그로 가지고 있는 Objects와의 거리를 산출
				float Distance = FVector::Dist(ActorLocation, Object->GetActorLocation());
				if (Distance < ClosestDistance)
				{
					// 가장 가까운 오브젝트를 변수에 할당하기 위함.
					ClosestDistance = Distance;
					ClosestObject = Object;
				}
			}

			// 결과를 블랙보드에 저장
			if (ClosestObject)
			{
				BlackboardComp->SetValueAsObject("TargetActor", ClosestObject);
				BlackboardComp->SetValueAsVector("TargetLocation", ClosestObject->GetActorLocation());
				Cast<ATestFood>(ClosestObject)->SetSelected(true);
				UE_LOG(LogTemp, Warning, TEXT("AnimalController::TargetLocation 업데이트 성공. %f, %f, %f"), ClosestObject->GetActorLocation().X, ClosestObject->GetActorLocation().Y, ClosestObject->GetActorLocation().Z );
				return Super::ExecuteTask(OwnerComp, NodeMemory);
			}
		}
		
		ActorLocation = GenerateRandomLocation(ActorLocation, WanderRange);
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsVector("TargetLocation", ActorLocation);
			UE_LOG(LogTemp, Warning, TEXT("AnimalController::TargetLocation 업데이트 성공. %f, %f, %f"), ActorLocation.X, ActorLocation.Y, ActorLocation.Z );
		}
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

FVector UBTTask_FindPoint::GenerateRandomLocation(const FVector& BaseLocation, float Range)
{
	const int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomX = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	const float RandomY = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	return BaseLocation + FVector(RandomX, RandomY, 0.f);
}