// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_FindFood.h"

#include "AIController.h"
#include "AIAnimal/TestFood.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryManager.h"

UBTTask_FindFood::UBTTask_FindFood()
{
	NodeName = TEXT("FindFood");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_FindFood::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	FoodQuery,
	AIPawn,
	 EEnvQueryRunMode::SingleResult,
	 nullptr);
    
	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UBTTask_FindFood::OnFindFoodQueryFinished);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
	
}

void UBTTask_FindFood::OnFindFoodQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	//성공하지 않았으면 리턴
	if (EEnvQueryStatus::Success != QueryStatus)
	{
		FinishLatentTask(*BTComp, EBTNodeResult::Failed);
		return;
	}
    
	if (BlackboardComp)
	{
		AActor* TargetItem = QueryInstance->GetQueryResult()->GetItemAsActor(0);
		if (TargetItem)
		{
			
			BlackboardComp->SetValueAsObject("NTargetFood", TargetItem);
			BlackboardComp->SetValueAsVector("NTargetFoodLocation", TargetItem->GetActorLocation());
			FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
			return;
		}
		FinishLatentTask(*BTComp, EBTNodeResult::Failed);
		return;
	}
	
	FinishLatentTask(*BTComp, EBTNodeResult::Failed);
}

//{
 		// FVector ActorLocation = AIPawn->GetActorLocation();
 	// // 가장 가까운 오브젝트 찾기
 	// AActor* ClosestObject = nullptr;
 	// float ClosestDistance = SearchRadius;
 	//
 	// TArray<AActor*> AllActors;
 	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATestFood::StaticClass(), AllActors);
 	//
 	// for (AActor* Object : AllActors)
 	// {
 	// 	if (Object->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
 	// 	{
 	// 		if (const IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Object))
 	// 		{
 	// 			FGameplayTagContainer TagContainer;
 	// 			TagInterface->GetOwnedGameplayTags(TagContainer);
 	// 			if (TagContainer.HasTag(FGameplayTag::RequestGameplayTag("Trigger.Animal.Interact.Harvest")))  
 	// 			{
 	// 				//다른 객체와 이미 상호작용중이면 다음으로 넘어감
 	// 				if (Cast<ATestFood>(Object)->GetSelected())
 	// 				{
 	// 					continue;
 	// 				}
 	//
 	// 				// 해당 AI Controller를 보유한 Actor의 Location과, Interactive를 태그로 가지고 있는 Objects와의 거리를 산출
 	// 				float Distance = FVector::Dist(ActorLocation, Object->GetActorLocation());
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
 	//
 	// // 결과를 블랙보드에 저장
 	// if (ClosestObject)
 	// {
 	// 	SearchRadius = 30000.f; 
 	// 	BlackboardComp->SetValueAsObject("NTargetFood", ClosestObject);
 	// 	BlackboardComp->SetValueAsVector("NTargetFoodLocation", ClosestObject->GetActorLocation());
 	// 	Cast<ATestFood>(ClosestObject)->SetSelected(true);
 	// 	return Super::ExecuteTask(OwnerComp, NodeMemory);
 	// }
 	//
 	// //주변에 푸드가 없으면
 	// SearchRadius += 1000.f;
 	// return EBTNodeResult::Failed;
 	//}