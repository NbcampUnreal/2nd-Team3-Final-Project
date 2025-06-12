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
		FVector TargetLocation = QueryInstance->GetQueryResult()->GetItemAsLocation(0);
		AActor*  TargetActor = QueryInstance->GetQueryResult()->GetItemAsActor(0);
		BlackboardComp->SetValueAsObject("TargetActor", TargetActor);
		BlackboardComp->SetValueAsVector("TargetLocation", TargetLocation);
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

	//게임플레이태그인터페이스 상속받은 엑터들 가져옴
 	//가져온 엑터들 중에 리더 태그 있는지 확인
 	//리더가 있으면 리더 주위로 이동
 	//리더가 없으면 경비들중 가장 가까운 애 주위로 이동
 	// 가장 가까운 오브젝트 찾기
 	
 	//
 	// AActor* ClosestObject = nullptr;
 	// const float WanderRange = BlackboardComp->GetValueAsFloat("WanderRange");
 	//
 	// TArray<AActor*> AllActors;
 	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseAIAnimal::StaticClass(), AllActors);
 	// float ClosestDistance = 20000.0f;
 	// for (AActor* Object : AllActors)
 	// {
 	// 	if (Object->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
 	// 	{
 	// 		if (const IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Object))
 	// 		{
 	// 			FGameplayTagContainer TagContainer;
 	// 			TagInterface->GetOwnedGameplayTags(TagContainer);
 	//
 	// 			//리더가 있을 때
 	// 			if (TagContainer.HasTag(FGameplayTag::RequestGameplayTag("Animal.Group.Leader"))) //Animal.Group.Alone , Animal.Group.Leader, Animal.Group.Patrol, Animal.Group.Follower
 	// 			{
 	//
 	// 				float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(), Object->GetActorLocation());
 	// 				if (Distance < ClosestDistance)
 	// 				{
 	// 					// 가장 가까운 오브젝트를 변수에 할당하기 위함.
 	// 					ClosestDistance = Distance;
 	// 					ClosestObject = Object;
 	// 				}
 	// 				if (ClosestObject)
 	// 				{
 	// 					FVector ActorLocation= GenerateRandomLocation(ClosestObject->GetActorLocation(), WanderRange);
 	// 					BlackboardComp->SetValueAsVector("TargetLocation", ActorLocation);
 	// 					return Super::ExecuteTask(OwnerComp, NodeMemory);
 	// 				}
 	// 			}
 	// 		}
 	// 	}
 	// }
 	//
 	// for (AActor* Object : AllActors)
 	// {
 	// 	if (Object->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
 	// 	{
 	// 		if (const IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Object))
 	// 		{
 	// 			FGameplayTagContainer TagContainer;
 	// 			TagInterface->GetOwnedGameplayTags(TagContainer);
 	//
 	// 			//리더가 없을 때
 	// 			if (TagContainer.HasTag(FGameplayTag::RequestGameplayTag("Animal.Group.Patrol"))) //Animal.Group.Alone , Animal.Group.Leader, Animal.Group.Patrol, Animal.Group.Follower
 	// 			{
 	// 				float Distance = FVector::Dist(AIPawn->GetActorLocation(), Object->GetActorLocation());
 	// 				if (Distance < ClosestDistance)
 	// 				{
 	// 					// 가장 가까운 오브젝트를 변수에 할당하기 위함.
 	// 					ClosestDistance = Distance;
 	// 					ClosestObject = Object;
 	// 				}
 	// 			}
 	// 		}
 	// 	}
 	// }