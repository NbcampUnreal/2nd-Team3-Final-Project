#include "QuestReceiverComponent.h"
#include "TimerManager.h"
#include "QuestDataRow.h" 
#include "PlayerQuestWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/EmberMainHUD.h"
#include "Engine/DataTable.h"

UQuestReceiverComponent::UQuestReceiverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    QuestLogWidget = nullptr;
    PlayerQuestWidget = nullptr;
}

void UQuestReceiverComponent::SetQuestLogWidget(UPlayerQuestWidget* InWidget)
{
    QuestLogWidget = InWidget;
    PlayerQuestWidget = InWidget;
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
    NewQuest.ObjectiveGoals = QuestRow->ObjectiveGoals;
    NewQuest.ObjectiveProgress.Init(0, QuestRow->ObjectiveGoals.Num());
    NewQuest.bIsTracking = true;
    NewQuest.bIsComplete = false;

    AddTrackedQuest(NewQuest);

    if (QuestLogWidget)
    {
        QuestLogWidget->SetQuestInfoFromDataRow(*QuestRow);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT(">> QuestLogWidget이 연결되지 않음"));
    }

    LastAcceptedQuestData = *QuestRow;
}

void UQuestReceiverComponent::CompleteQuest(int32 QuestID)
{
    int32 Index = FindQuestIndex(QuestID);
    if (Index == INDEX_NONE || QuestLog[Index].bIsComplete)
    {
        return; // 이미 완료된 퀘스트면 처리 안 함
    }
        FQuestStorageInfo& CompletedQuest = QuestLog[Index];
        CompletedQuest.bIsComplete = true;
        CompletedQuest.bIsTracking = false;

        OnQuestCompleted.Broadcast(CompletedQuest);
    
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

bool UQuestReceiverComponent::IsQuestComplete(int32 QuestID) const
{
    int32 Index = FindQuestIndex(QuestID);
    return (Index != INDEX_NONE) ? QuestLog[Index].bIsComplete : false;
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
//대화 목표 퀘스트
void UQuestReceiverComponent::NotifyTalkObjectiveCompleted(AActor* TalkedNPC)
{
    if (!TalkedNPC) return;

    for (int32 i = 0; i < QuestLog.Num(); ++i)
    {
        FQuestStorageInfo& Quest = QuestLog[i];

        for (int32 j = 0; j < Quest.ObjectiveNames.Num(); ++j)
        {
            FName RequiredTag(*Quest.ObjectiveNames[j]);

            if (TalkedNPC->ActorHasTag(RequiredTag))
            {
                Quest.ObjectiveProgress[j] = FMath::Clamp(Quest.ObjectiveProgress[j] + 1, 0, Quest.ObjectiveGoals.IsValidIndex(j) ? Quest.ObjectiveGoals[j] : 1);
                OnQuestUpdated.Broadcast(Quest);

                bool bAllObjectivesComplete = true;
                for (int32 k = 0; k < Quest.ObjectiveProgress.Num(); ++k)
                {
                    if (Quest.ObjectiveProgress[k] < Quest.ObjectiveGoals[k])
                    {
                        bAllObjectivesComplete = false;
                        break;
                    }
                }
                if (bAllObjectivesComplete && !Quest.bIsComplete)
                {
                    CompleteQuest(Quest.QuestID);
                }

                return;
            }
        }
    }
}
EDialogueStage UQuestReceiverComponent::GetDialogueStageForQuest(FName QuestRowName, UDataTable* QuestDataTable)
{
    if (!QuestDataTable || QuestRowName.IsNone())
        return EDialogueStage::BeforeAccept;

    const FQuestDataRow* Row = QuestDataTable->FindRow<FQuestDataRow>(QuestRowName, TEXT("StageCheck"));
    if (!Row) return EDialogueStage::BeforeAccept;

    int32 QuestID = Row->QuestID;

    if (IsQuestComplete(QuestID))
        return EDialogueStage::AfterComplete;

    const FQuestStorageInfo* Found = QuestLog.FindByPredicate([&](const FQuestStorageInfo& Q)
        {
            return Q.QuestID == QuestID;
        });

    if (Found)
    {
        bool bAllObjectivesMet = true;
        for (int32 i = 0; i < Found->ObjectiveGoals.Num(); ++i)
        {
            if (Found->ObjectiveProgress[i] < Found->ObjectiveGoals[i])
            {
                bAllObjectivesMet = false;
                break;
            }
        }
        return bAllObjectivesMet ? EDialogueStage::BeforeComplete : EDialogueStage::InProgress;
    }

    return EDialogueStage::BeforeAccept;
}
