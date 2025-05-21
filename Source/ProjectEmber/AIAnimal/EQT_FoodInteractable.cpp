// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/EQT_FoodInteractable.h"
#include "AIAnimal/TestFood.h"
#include "GameplayTagAssetInterface.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Direction.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

UEQT_FoodInteractable::UEQT_FoodInteractable()
{
	Cost = EEnvTestCost::Low; //테스트의 처리 비용을 정의합니다. Low로 설정하면 가벼운 테스트로 간주 -> 실제로 무거우면? 무슨 용도지?
	ValidItemType = UEnvQueryItemType_Actor::StaticClass(); //이 테스트가 적용 가능한 아이템 타입을 설정합니다. Actor만 검사 대상으로 삼겠다는 의미
	//UEnvQueryItemType_Actor, UEnvQueryItemType_ActorBase, UEnvQueryItemType_VectorBase, UEnvQueryItemType_Direction, UEnvQueryItemType_Point
	//Actor, ActorBase, VectorBase 는 머임, base는 왜 자꾸 붙지
}

void UEQT_FoodInteractable::RunTest(FEnvQueryInstance& QueryInstance) const 
{
	Super::RunTest(QueryInstance);

	//각 후보 아이템에 대해 통과 여부 또는 스코어를 판단
	
	UObject* Querier = QueryInstance.Owner.Get(); //쿼리를 실행한 주체 가져옴 -> AI Controller나 Pawn 이라는데 지금은 AI Controller 같음
	if (!Querier) return;

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It) //왜,,,왜 이터레이터요?
	{
		AActor* Actor = Cast<AActor>(QueryInstance.GetItemAsActor(It.GetIndex())); //인덱스를 기반으로 해당 아이템을 Actor로 캐스팅해서 가져옴(현재 반복 중인 아이템의 인덱스를 가져옴)
		if (!Actor) continue;

		ATestFood* Food = Cast<ATestFood>(Actor);
		if (!Food || Food->GetSelected()) //식량이 아니거나, 이미 다른 AI가 선택한 상태라면
		{
			It.ForceItemState(EEnvItemStatus::Failed); //현재 항목을 EQS 후보군에서 제외(실패 처리) EEnvItemStatus::Passed
			continue; 
		}

		if (Actor->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass())) //Actor가 GameplayTag 인터페이스를 구현하고 있는지 확인
		{
			const IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Actor);
			FGameplayTagContainer Tags;
			TagInterface->GetOwnedGameplayTags(Tags); // Actor가 소유한 모든 GameplayTag를 Tags 컨테이너에 가져옴

			if (Tags.HasTag(FGameplayTag::RequestGameplayTag("Trigger.Animal.Interact.Harvest")))
			{
				It.SetScore(TestPurpose, FilterType, 1.0f, 0.f);  // 통과
			}
			else
			{
				It.ForceItemState(EEnvItemStatus::Failed);
			}
		}
	}
}
