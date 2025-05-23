// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestReceiverComponent.generated.h"

USTRUCT(BlueprintType)
struct FQuestStorageInfo : public FTableRowBase
{
    GENERATED_BODY()

    FQuestStorageInfo()
        : QuestID(0)
        , QuestName(TEXT("Unnamed Quest"))
        , ObjectiveNames()
        , ObjectiveProgress()
        , ObjectiveGoals()
        , bIsTracking(false)
        , bIsComplete(false)
    {
    }

    UPROPERTY(BlueprintReadWrite)
    int32 QuestID;

    UPROPERTY(BlueprintReadWrite)
    FString QuestName;

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> ObjectiveNames; 

    UPROPERTY(BlueprintReadWrite)
    TArray<int32> ObjectiveProgress; 

    UPROPERTY(BlueprintReadWrite)
    TArray<int32> ObjectiveGoals;    

    UPROPERTY(BlueprintReadWrite)
    bool bIsTracking;

    UPROPERTY(BlueprintReadWrite)
    bool bIsComplete;
}; 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestEventSignature, const FQuestStorageInfo&, QuestInfo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UQuestReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UQuestReceiverComponent();

    UFUNCTION(BlueprintCallable)
    void AcceptQuest(UDataTable* QuestDataTable, FName RowName);

    UFUNCTION(BlueprintCallable)
    void AddTrackedQuest(const FQuestStorageInfo& Quest);

    UFUNCTION(BlueprintCallable)
    void RemoveTrackedQuest(const FQuestStorageInfo& Quest);

    UFUNCTION(BlueprintCallable)
    void CompleteQuest(int32 QuestID);

    UFUNCTION(BlueprintCallable)
    void AbandonQuest(int32 QuestID);

    UFUNCTION(BlueprintCallable)
    void UpdateQuestObjective(int32 QuestID, const FString& ObjectiveName, int32 QuantityIncrease);

    UFUNCTION(BlueprintCallable)
    void RemoveTrackingObjective(int32 QuestID, const FString& ObjectiveName);

    UFUNCTION(BlueprintCallable)
    const TArray<FQuestStorageInfo>& GetQuestLog() const;

    UFUNCTION(BlueprintCallable)
    void NotifyTalkObjectiveCompleted(AActor* TalkedNPC);

    UPROPERTY(BlueprintAssignable)
    FQuestEventSignature OnQuestAccepted;

    UPROPERTY(BlueprintAssignable)
    FQuestEventSignature OnQuestAbandoned;

    UPROPERTY(BlueprintAssignable)
    FQuestEventSignature OnQuestCompleted;

    UPROPERTY(BlueprintAssignable)
    FQuestEventSignature OnQuestUpdated;

    bool IsQuestComplete(int32 QuestID) const;

private:
    UPROPERTY()
    TArray<FQuestStorageInfo> QuestLog;

    int32 FindQuestIndex(int32 QuestID) const;
};
