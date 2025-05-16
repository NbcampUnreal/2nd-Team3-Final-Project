#include "QuestReceiverComponent.h"
#include "TimerManager.h"

UQuestReceiverComponent::UQuestReceiverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}



void UQuestReceiverComponent::AddTrackedQuest(const FQuestStorageInfo& Quest)
{
    int32 Index = FindQuestIndex(Quest.QuestNumber);
    if (Index == INDEX_NONE)
    {
        QuestLog.Add(Quest);
        OnQuestAccepted.Broadcast(Quest);
    }
    else
    {
        QuestLog[Index].bIsTracking = true;
        OnQuestUpdated.Broadcast(QuestLog[Index]);
    }
}

void UQuestReceiverComponent::RemoveTrackedQuest(const FQuestStorageInfo& Quest)
{
    int32 Index = FindQuestIndex(Quest.QuestNumber);
    if (Index != INDEX_NONE)
    {
        QuestLog[Index].bIsTracking = false;
        OnQuestUpdated.Broadcast(QuestLog[Index]);
    }
}

void UQuestReceiverComponent::CompleteQuest(int32 QuestID)
{
    int32 Index = FindQuestIndex(QuestID);
    if (Index != INDEX_NONE)
    {
        QuestLog[Index].bIsComplete = true;
        QuestLog[Index].bIsTracking = false;
        OnQuestCompleted.Broadcast(QuestLog[Index]);
    }
}

void UQuestReceiverComponent::AbandonQuest(int32 QuestID)
{
    int32 Index = FindQuestIndex(QuestID);
    if (Index != INDEX_NONE)
    {
        FQuestStorageInfo AbandonedQuest = QuestLog[Index];
        QuestLog.RemoveAt(Index);
        OnQuestAbandoned.Broadcast(AbandonedQuest);
    }
}

void UQuestReceiverComponent::UpdateQuestObjective(int32 QuestID, const FString& ObjectiveName, int32 QuantityIncrease)
{
    int32 Index = FindQuestIndex(QuestID);
    if (Index != INDEX_NONE)
    {
        FQuestStorageInfo& Quest = QuestLog[Index];
        for (int32 i = 0; i < Quest.ObjectiveNames.Num(); ++i)
        {
            if (Quest.ObjectiveNames[i] == ObjectiveName)
            {
                Quest.ObjectiveProgress[i] += QuantityIncrease;
                break;
            }
        }
        OnQuestUpdated.Broadcast(Quest);
    }
}

void UQuestReceiverComponent::RemoveTrackingObjective(int32 QuestID, const FString& ObjectiveName)
{
    int32 Index = FindQuestIndex(QuestID);
    if (Index != INDEX_NONE)
    {
        FQuestStorageInfo& Quest = QuestLog[Index];
        for (int32 i = 0; i < Quest.ObjectiveNames.Num(); ++i)
        {
            if (Quest.ObjectiveNames[i] == ObjectiveName)
            {
                Quest.ObjectiveProgress[i] = 0;
                break;
            }
        }
        OnQuestUpdated.Broadcast(Quest);
    }
}

int32 UQuestReceiverComponent::FindQuestIndex(int32 QuestID) const
{
    for (int32 i = 0; i < QuestLog.Num(); ++i)
    {
        if (QuestLog[i].QuestNumber == QuestID)
        {
            return i;
        }
    }
    return INDEX_NONE;
}

const TArray<FQuestStorageInfo>& UQuestReceiverComponent::GetQuestLog() const
{
    return QuestLog;
}