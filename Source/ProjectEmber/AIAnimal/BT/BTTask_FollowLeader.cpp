// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_FollowLeader.h"

#include "AIController.h"
#include "GameplayTagAssetInterface.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "AIAnimal/TestFood.h"
#include "BehaviorTree/BlackboardComponent.h"
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

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	
	//게임플레이태그인터페이스 상속받은 엑터들 가져옴
	//가져온 엑터들 중에 리더 태그 있는지 확인
	//리더가 있으면 리더 주위로 이동
	//리더가 없으면 경비들중 가장 가까운 애 주위로 이동
	// 가장 가까운 오브젝트 찾기
	
	
	AActor* ClosestObject = nullptr;
	const float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange");
	
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseAIAnimal::StaticClass(), AllActors);
	float ClosestDistance = 20000.0f;
	for (AActor* Object : AllActors)
	{
		if (Object->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
		{
			if (const IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Object))
			{
				FGameplayTagContainer TagContainer;
				TagInterface->GetOwnedGameplayTags(TagContainer);

				//리더가 있을 때
				if (TagContainer.HasTag(FGameplayTag::RequestGameplayTag("Animal.Group.Leader"))) //Animal.Group.Alone , Animal.Group.Leader, Animal.Group.Patrol, Animal.Group.Follower
				{

					float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(), Object->GetActorLocation());
					if (Distance < ClosestDistance)
					{
						// 가장 가까운 오브젝트를 변수에 할당하기 위함.
						ClosestDistance = Distance;
						ClosestObject = Object;
					}
					if (ClosestObject)
					{
						FVector ActorLocation= GenerateRandomLocation(ClosestObject->GetActorLocation(), WanderRange);
						BlackboardComp->SetValueAsVector("TargetLocation", ActorLocation);
						return Super::ExecuteTask(OwnerComp, NodeMemory);
					}
				}
			}
		}
	}
	
	for (AActor* Object : AllActors)
	{
		if (Object->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
		{
			if (const IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Object))
			{
				FGameplayTagContainer TagContainer;
				TagInterface->GetOwnedGameplayTags(TagContainer);

				//리더가 없을 때
				if (TagContainer.HasTag(FGameplayTag::RequestGameplayTag("Animal.Group.Patrol"))) //Animal.Group.Alone , Animal.Group.Leader, Animal.Group.Patrol, Animal.Group.Follower
				{
					float Distance = FVector::Dist(AIPawn->GetActorLocation(), Object->GetActorLocation());
					if (Distance < ClosestDistance)
					{
						// 가장 가까운 오브젝트를 변수에 할당하기 위함.
						ClosestDistance = Distance;
						ClosestObject = Object;
					}
				}
			}
		}
	}

	// 결과를 블랙보드에 저장
	if (ClosestObject)
	{
		BlackboardComp->SetValueAsVector("TargetLocation", ClosestObject->GetActorLocation());
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

FVector UBTTask_FollowLeader::GenerateRandomLocation(const FVector& BaseLocation, const float Range)
{
	int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomX = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomY = RandomSign * FMath::RandRange(0.1f, 1.0f) * Range;
	return BaseLocation + FVector(RandomX, RandomY, 0.f);
}