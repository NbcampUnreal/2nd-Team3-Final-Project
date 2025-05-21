#include "QuestReceiverComponent.h"
#include "TimerManager.h"
#include "QuestDataRow.h" 
#include "Engine/DataTable.h"

UQuestReceiverComponent::UQuestReceiverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UQuestReceiverComponent::AcceptQuest(UDataTable* QuestDataTable, FName RowName)
{
    if (!QuestDataTable) return;

    const FQuestDataRow* QuestRow = QuestDataTable->FindRow<FQuestDataRow>(RowName, TEXT("AcceptQuest"));
    if (!QuestRow) return;

    FQuestStorageInfo NewQuest;
    NewQuest.QuestID = QuestRow->QuestID;
    NewQuest.QuestName = QuestRow->QuestName;
    NewQuest.ObjectiveNames = QuestRow->ObjectiveNames;
    NewQuest.ObjectiveProgress.Init(0, QuestRow->ObjectiveGoals.Num());
    NewQuest.bIsTracking = true;
    NewQuest.bIsComplete = false;

    AddTrackedQuest(NewQuest);
}

void UQuestReceiverComponent::AddTrackedQuest(const FQuestStorageInfo& Quest)
{
    int32 Index = FindQuestIndex(Quest.QuestID);
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
    int32 Index = FindQuestIndex(Quest.QuestID);
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
        if (QuestLog[i].QuestID == QuestID)
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
void UQuestReceiverComponent::NotifyTalkObjectiveCompleted(FName ObjectiveName)
{
    FString ObjectiveNameStr = ObjectiveName.ToString();

    for (int32 i = 0; i < QuestLog.Num(); ++i)
    {
        FQuestStorageInfo& Quest = QuestLog[i];
        for (int32 j = 0; j < Quest.ObjectiveNames.Num(); ++j)
        {
            if (Quest.ObjectiveNames[j] == ObjectiveNameStr)
            {
                Quest.ObjectiveProgress[j] = FMath::Clamp(Quest.ObjectiveProgress[j] + 1, 0, 1);
                OnQuestUpdated.Broadcast(Quest);
                return;
            }
        }
    }
}