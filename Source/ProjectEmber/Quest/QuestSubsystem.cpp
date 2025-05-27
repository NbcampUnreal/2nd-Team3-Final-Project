#include "QuestSubsystem.h"
#include "Data/QuestDataAsset.h"
#include "EmberLog/EmberLog.h"
#include "GameInstance/GameplayEventSubsystem.h"

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadAllQuests();
    
    if (UGameplayEventSubsystem* EventSubsystem = UGameplayEventSubsystem::Get(GetWorld()))
    {
        EventSubsystem->OnGameEvent.AddDynamic(this, &UQuestSubsystem::OnGameEvent);
    }
}

void UQuestSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UQuestSubsystem::LoadAllQuests()
{
    for (auto& SoftAsset : QuestAssets)
    {
        if (UQuestDataAsset* Asset = SoftAsset.IsValid() ? SoftAsset.Get() : SoftAsset.LoadSynchronous())
        {
            LoadedQuests.Add(Asset->QuestID, Asset);
        }
    }
}

bool UQuestSubsystem::TryStartQuest(FName QuestID)
{
    if (LoadedQuests.Contains(QuestID) && !QuestProgress.Contains(QuestID) && !CompletedQuests.Contains(QuestID))
    {
        QuestProgress.Add(QuestID, 0);
        EMBER_LOG(LogEmber, Log, TEXT("Quest %s started."), *QuestID.ToString());
        return true;
    }
    return false;
}

void UQuestSubsystem::OnGameEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
    for (const auto& Pair : QuestProgress)
    {
        if (UQuestDataAsset* QuestAsset = LoadedQuests.FindRef(Pair.Key))
        {
            CheckQuestStepCompletion(QuestAsset, EventTag, EventData);
        }
    }
}

void UQuestSubsystem::CheckQuestStepCompletion(UQuestDataAsset* QuestAsset, const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
    int32& StepIndex = QuestProgress.FindChecked(QuestAsset->QuestID);
    const TArray<FQuestStep>& Steps = QuestAsset->Steps;
    if (!Steps.IsValidIndex(StepIndex))
    {
        return;
    }

    const FQuestStep& CurrentStep = Steps[StepIndex];

    // 이벤트 발생 시 각 UQuestCondition에 전달하여 카운트 업데이트
    for (UQuestCondition* Condition : CurrentStep.Conditions)
    {
        if (Condition && Condition->EventTag == EventTag)
        {
            Condition->OnEvent(EventTag, EventData);
        }
    }

    // 모든 조건이 충족됐는지 확인
    bool bAllConditionsMet = true;
    for (const UQuestCondition* Condition : CurrentStep.Conditions)
    {
        if (!Condition || !Condition->IsFulfilled())
        {
            bAllConditionsMet = false;
            break;
        }
    }

    if (bAllConditionsMet)
    {
        AdvanceQuestStep(QuestAsset->QuestID);
    }
}

bool UQuestSubsystem::AdvanceQuestStep(FName QuestID)
{
    if (!QuestProgress.Contains(QuestID))
    {
        return false;
    }

    int32& Index = QuestProgress.FindChecked(QuestID);
    UQuestDataAsset* QuestAsset = LoadedQuests.FindRef(QuestID);
    if (!QuestAsset)
    {
        return false;
    }

    Index++;
    int32 NumSteps = QuestAsset->Steps.Num();
    if (Index >= NumSteps)
    {
        return CompleteQuest(QuestID);
    }
    EMBER_LOG(LogEmber, Log, TEXT("Quest %s advanced to step %d."), *QuestID.ToString(), Index);
    return true;
}

bool UQuestSubsystem::CompleteQuest(FName QuestID)
{
    if (!QuestProgress.Contains(QuestID))
    {
        return false;
    }

    UQuestDataAsset* QuestAsset = LoadedQuests.FindRef(QuestID);
    if (!QuestAsset)
    {
        return false;
    }
    
    for (const FQuestRewardData& Reward : QuestAsset->Steps.Last().Rewards)
    {
        /**
         * 각 리워드 보상타입에 맞춰서 함수 호출해주면 될듯
         * AddItem이라던가 경험치면 GameplayEffect를 호출한다던가
         */
    }

    QuestProgress.Remove(QuestID);
    CompletedQuests.Add(QuestID);
    EMBER_LOG(LogEmber, Log, TEXT("Quest %s completed."), *QuestID.ToString());
    return true;
}
