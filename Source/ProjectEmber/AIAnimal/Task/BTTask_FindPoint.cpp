// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectEmber/AIAnimal/Task/BTTask_FindPoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

UBTTask_FindPoint::UBTTask_FindPoint()
{
	NodeName = TEXT("FindPoint");
}

EBTNodeResult::Type UBTTask_FindPoint::ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory)
{
	AAIController* AIController = Comp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = Comp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	FVector ActorLocation = AIPawn->GetActorLocation();
	const float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange");

	const bool bIsHungry = BlackboardComp->GetValueAsBool("IsHungry");
	const uint8 CurrentState = BlackboardComp->GetValueAsEnum("CurrentState");

	if (CurrentState == static_cast<uint8>(EAnimalAIState::Idle))
	{
		if (bIsHungry)
		{
			// 상호작용 가능한 가장 가까운 대상 찾기 -> 다른 객체와 타겟 겹치지 않게 처리해야함
			TArray<AActor*> InteractiveObjects;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), InteractiveObjectTag, InteractiveObjects);

			// 가장 가까운 오브젝트 찾기
			AActor* ClosestObject = nullptr;
			float ClosestDistance = SearchRadius;
			FVector AILocation = AIPawn->GetActorLocation();

			for (AActor* Object : InteractiveObjects)
			{   // 해당 AI Controller를 보유한 Actor의 Location과, Interactive를 태그로 가지고 있는 Objects와의 거리를 산출
				float Distance = FVector::Dist(AILocation, Object->GetActorLocation());
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
				BlackboardComp->SetValueAsObject(GetSelectedBlackboardKey(), ClosestObject);
				BlackboardComp->SetValueAsVector("TargetLocation", ClosestObject->GetActorLocation());
				BlackboardComp->SetValueAsEnum("CurrentState", static_cast<uint8>(EAnimalAIState::Wander));
				UE_LOG(LogTemp, Warning, TEXT("AnimalController::TargetLocation 업데이트 성공. %f, %f, %f"), ClosestObject->GetActorLocation().X, ClosestObject->GetActorLocation().Y, ClosestObject->GetActorLocation().Z );
				return Super::ExecuteTask(Comp, NodeMemory);
			}
			//먹이의 상호작용 호출하는 task 만들어야함 ->먹이셀프삭제, 동물 헝그리설정 변경
		}
		// 어슬렁거리기 로직
		const int RandomSign = FMath::RandRange(0,1);
		const float RandomWanderRangeX = FMath::RandRange(0.1f, 1.0f);
		const float RandomWanderRangeY = FMath::RandRange(0.1f, 1.0f);
		if (RandomSign == 1)
		{
			ActorLocation.X += WanderRange * RandomWanderRangeX;
			ActorLocation.Y += WanderRange * RandomWanderRangeY;
		}
		else
		{
			ActorLocation.X -= WanderRange * RandomWanderRangeX;
			ActorLocation.Y -= WanderRange * RandomWanderRangeY;
		}
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsVector("TargetLocation", ActorLocation);
			UE_LOG(LogTemp, Warning, TEXT("AnimalController::TargetLocation 업데이트 성공. %f, %f, %f"), ActorLocation.X, ActorLocation.Y, ActorLocation.Z );
		}
		return Super::ExecuteTask(Comp, NodeMemory);
	}
	
	return Super::ExecuteTask(Comp, NodeMemory);
}
