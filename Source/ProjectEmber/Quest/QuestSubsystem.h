#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Setting/QuestDataSetting.h"
#include "QuestSubsystem.generated.h"

class UQuestDataAsset;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStarted, UQuestDataAsset*, QuestAsset);


UCLASS(BlueprintType)
class PROJECTEMBER_API UQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	int32 GetCurrentStepIndexForQuest(FName QuestID, bool bAutoStartIfNotExists = false);


public:
	// NPC에게 F눌러서 통과 받을때 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool TryStartQuest(FName QuestID, bool bPlayerAccepted = false);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool IsQuestAccepted(FName QuestID) const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AcceptStep(FName QuestID);

	//마지막으로 수락한 퀘스트
	UPROPERTY()
	FName LastAcceptedQuestID = NAME_None;

	// 특정 행동 (동물죽엇다, 뭐 파밍햇다, 공격햇다)에 대한 이벤트 호출 함수
	UFUNCTION()
	void OnGameEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);

	/* 연계퀘스트에 다음 퀘스트로 넘어가는 함수 
	 * (다음 퀘스트는 실제로 퀘스트를 받는게 아닌 NPC의 느낌표를 활성화 해주는 식으로 접근 하면 됨)
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AdvanceQuestStep(FName QuestID);

	// 하나의 연계퀘스트가 완료되면 호출될 함수 ( 수정 필요 )
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool CompleteQuest(FName QuestID);

	// 단순 완료된 퀘스트인지 검사하는 함수
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool IsQuestCompleted(FName QuestID) const;

	TMap<FName, TObjectPtr<UQuestDataAsset>>& GetAllLoadedQuests();

	bool GetLastActiveQuestID(FName& OutQuestID) const;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestStarted OnQuestStarted;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool IsStepCompleted(FName QuestID, int32 StepIndex) const;

	TMap<FName, bool> StepAcceptance;

private:
	// 로드된 퀘스트 목록
	UPROPERTY(VisibleAnywhere, Category = "Quest", meta = (AllowPrivateAccess = "true"))
	TMap<FName, TObjectPtr<UQuestDataAsset>> LoadedQuests;

	// 현재 진행 중인 퀘스트 목록
	TMap<FName, int32> QuestProgress;

	// 완료된 퀘스트 목록
	TSet<FName> CompletedQuests;

	// 초기화 단계에서 데이터불러오는 함수
	void LoadAllQuests();
		
	// OnGameEvent가 호출되면 QuestProgress를 순회하면서 이 함수를 호출
	void CheckQuestStepCompletion(const UQuestDataAsset* QuestAsset, const FGameplayTag& EventTag, const FGameplayEventData& EventData);
	
};

inline bool UQuestSubsystem::IsQuestCompleted(FName QuestID) const
{
	return CompletedQuests.Contains(QuestID);
}
