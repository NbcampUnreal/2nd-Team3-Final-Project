#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "QuestSubsystem.generated.h"

class UQuestDataAsset;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool TryStartQuest(FName QuestID);

	UFUNCTION()
	void OnGameEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
	
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AdvanceQuestStep(FName QuestID);
	
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool CompleteQuest(FName QuestID);

private:
	// 퀘스트 데이터 에셋 목록
	UPROPERTY(EditDefaultsOnly, Category = "Quest|Config")
	TArray<TSoftObjectPtr<UQuestDataAsset>> QuestAssets;

	// 로드된 퀘스트 목록
	TMap<FName, UQuestDataAsset*> LoadedQuests;

	// 현재 진행 중인 퀘스트 목록
	TMap<FName, int32> QuestProgress;

	// 완료된 퀘스트 목록
	TSet<FName> CompletedQuests;
	
	void LoadAllQuests();

	void CheckQuestStepCompletion(UQuestDataAsset* QuestAsset, const FGameplayTag& EventTag, const FGameplayEventData& EventData);
};
