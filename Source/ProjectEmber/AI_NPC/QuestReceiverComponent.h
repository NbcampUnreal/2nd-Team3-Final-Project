// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestReceiverComponent.generated.h"

USTRUCT(BlueprintType)
struct FQuestStorageInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 QuestNumber;

    UPROPERTY(BlueprintReadWrite)
    FString QuestName;

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> ObjectiveNames;

    UPROPERTY(BlueprintReadWrite)
    TArray<int32> ObjectiveProgress;

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

    // Quest Management
    UFUNCTION(BlueprintCallable)
    void AddTrackedQuest(const FQuestStorageInfo& Quest);

    UFUNCTION(BlueprintCallable)
    void RemoveTrackedQuest(const FQuestStorageInfo& Quest);

    UFUNCTION(BlueprintCallable)
    void CompleteQuest(int32 QuestID);

    UFUNCTION(BlueprintCallable)
    void AbandonQuest(int32 QuestID);

    // Objective Management
    UFUNCTION(BlueprintCallable)
    void UpdateQuestObjective(int32 QuestID, const FString& ObjectiveName, int32 QuantityIncrease);

    UFUNCTION(BlueprintCallable)
    void RemoveTrackingObjective(int32 QuestID, const FString& ObjectiveName);

    UFUNCTION(BlueprintCallable)
    const TArray<FQuestStorageInfo>& GetQuestLog() const;

    UPROPERTY(BlueprintAssignable)
    FQuestEventSignature OnQuestAccepted;

    UPROPERTY(BlueprintAssignable)
    FQuestEventSignature OnQuestAbandoned;

    UPROPERTY(BlueprintAssignable)
    FQuestEventSignature OnQuestCompleted;

    UPROPERTY(BlueprintAssignable)
    FQuestEventSignature OnQuestUpdated;


private:
    UPROPERTY()
    TArray<FQuestStorageInfo> QuestLog;

    int32 FindQuestIndex(int32 QuestID) const;
};
