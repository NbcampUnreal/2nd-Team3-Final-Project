#pragma once

#include "CoreMinimal.h"
#include "EMSActorSaveInterface.h"
#include "Components/ActorComponent.h"
#include "QuestSaveComponent.generated.h"

USTRUCT(BlueprintType)
struct FQuestSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FName QuestID;
	
	UPROPERTY()
	int32 CurrentStep = 0;
	
	UPROPERTY()
	TArray<int32> ConditionCounts;
};

/** 임시 세이브 컴포넌트 
 * 아예 BaseSaveComponent를 만들어서 배치할지 고민
 */

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UQuestSaveComponent : public UActorComponent, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	UQuestSaveComponent();

public:
	/**
	 * 이런 느낌으로 PlayerState나 GameInstance에서
	 * TArray<BaseSaveComponent*> SaveComponents;
	 * 를 순회하면서 함수 호출하면 되지 않을까 생각 중
	 */
	UPROPERTY()
	TArray<FQuestSaveData> ActiveQuests;
	//  virtual void CaptureSaveData(FEmberSaveData& OutSaveData) override
	//  {
	//  	OutSaveData.QuestData = ActiveQuests;
	//  }
	// virtual void ApplyLoadData(const FEmberSaveData& InSaveData) override
	// {
	// 	ActiveQuests = InSaveData.QuestData;
	// }
	
};
