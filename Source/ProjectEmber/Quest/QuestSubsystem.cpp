#include "QuestSubsystem.h"
#include "AbilitySystemComponent.h"
#include "AI_NPC/NPC_Component/QuestGiverComponent.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Character/EmberCharacter.h"
#include "Data/QuestDataAsset.h"
#include "EmberLog/EmberLog.h"
#include "GameInstance/GameplayEventSubsystem.h"
#include "Item/UserItemManger.h"

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadAllQuests();
    
    if (UGameplayEventSubsystem* EventSubsystem = UGameplayEventSubsystem::GetGameplayEvent(GetWorld()))
    {
        EventSubsystem->OnGameEvent.AddDynamic(this, &UQuestSubsystem::OnGameEvent);
    }
}
//서브시스템 종료시 씀
void UQuestSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UQuestSubsystem::LoadAllQuests()
{
    const UQuestDataSetting* QuestDataSetting = GetDefault<UQuestDataSetting>();
    for (auto& SoftAsset : QuestDataSetting->QuestAssets)
    {
        if (UQuestDataAsset* Asset = SoftAsset.IsValid() ? SoftAsset.Get() : SoftAsset.LoadSynchronous())
        {
            LoadedQuests.Add(Asset->QuestID, Asset);
        }
    }
}
//퀘스트 시작 등록
bool UQuestSubsystem::TryStartQuest(FName QuestID, bool bPlayerAccepted)
{
    UE_LOG(LogTemp, Warning, TEXT("!!! [TryStartQuest] QuestID: %s / bPlayerAccepted: %d"), *QuestID.ToString(), bPlayerAccepted);

    // 수락 의사가 없거나, 퀘스트가 존재하지 않거나 이미 완료됐으면 리턴
    if (!bPlayerAccepted || !LoadedQuests.Contains(QuestID) || CompletedQuests.Contains(QuestID))
    {
        return false;
    }

    // 퀘스트가 처음 시작되는 경우만 Step 0 초기화
    if (!QuestProgress.Contains(QuestID))
    {
        QuestProgress.Add(QuestID, 0);

        if (UQuestDataAsset* QuestAsset = LoadedQuests.FindRef(QuestID))
        {
            if (QuestAsset->Steps.IsValidIndex(0))
            {
                FQuestStep FirstStep = QuestAsset->Steps[0];
                for (UQuestCondition* Condition : FirstStep.Conditions)
                {
                    if (Condition)
                    {
                        Condition->CurrentCount = 0;
                    }
                }
            }
        }
    }

    LastAcceptedQuestID = QuestID;

    if (UQuestDataAsset* QuestAsset = LoadedQuests.FindRef(QuestID))
    {
        OnQuestStarted.Broadcast(QuestAsset);
    }

    return true;
}
//설정된 퀘스트 Tag로 조건검사
void UQuestSubsystem::OnGameEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
    UE_LOG(LogTemp, Warning, TEXT(" [UQuestSubsystem::OnGameEvent] Received EventTag: %s"), *EventTag.ToString());
    TArray<FName> KeysToCheck;
    QuestProgress.GenerateKeyArray(KeysToCheck);

    for (FName QuestID : KeysToCheck)
    {
        if (UQuestDataAsset* QuestAsset = LoadedQuests.FindRef(QuestID))
        {
            CheckQuestStepCompletion(QuestAsset, EventTag, EventData);
        }
    }
}

void UQuestSubsystem::CheckQuestStepCompletion(const UQuestDataAsset* QuestAsset, const FGameplayTag& EventTag, const FGameplayEventData& EventData)
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
}

bool UQuestSubsystem::AdvanceQuestStep(FName QuestID)
{
    if (!QuestProgress.Contains(QuestID))
    {
        return false;
    }
    //퀘스트 진행도가져옴
    int32& Index = QuestProgress.FindChecked(QuestID);
    UQuestDataAsset* QuestAsset = LoadedQuests.FindRef(QuestID);
    if (!QuestAsset)
    {
        return false;
    }

    if (QuestAsset->Steps.IsValidIndex(Index))
    {
        const FQuestStep& CompletedStep = QuestAsset->Steps[Index];
        for (const FQuestRewardData& Reward : CompletedStep.Rewards)
        {
            /**
             * 각 리워드 보상타입에 맞춰서 함수 호출해주면 될듯
             * AddItem이라던가 경험치면 GameplayEffect를 호출한다던가
             */
        }
    }
    
    Index++;
    int32 NumSteps = QuestAsset->Steps.Num();
    if (Index >= NumSteps)
    {
        return CompleteQuest(QuestID);
    }
    const FQuestStep& NextStep = QuestAsset->Steps[Index];
   
    for (UQuestCondition* Condition : NextStep.Conditions)
    {
        if (Condition)
        {
           Condition->CurrentCount = 0;
        }
    }

    if (AActor* GiverActor = NextStep.QuestGiver.Get())
    {
        if (UQuestGiverComponent* GiverComp = GiverActor->FindComponentByClass<UQuestGiverComponent>())
        {
            // 여기서 npc 상호작용 가능하게 키기
            /*f (!GiverComp->ExclamationMarkComponent)
            {
                GiverComp->ExclamationMarkComponent = NewObject<UStaticMeshComponent>(GiverActor);
                GiverComp->ExclamationMarkComponent->SetupAttachment(GiverComp->GetRootComponent());
                GiverComp->ExclamationMarkComponent->SetStaticMesh(GiverComp->ExclamationMarkMesh);
                GiverComp->ExclamationMarkComponent->RegisterComponent();
            }
            GiverComp->ExclamationMarkComponent->SetVisibility(true);*/
        }
    }
    
    EMBER_LOG(LogEmber, Log, TEXT("Quest %s advanced to step %d."), *QuestID.ToString(), Index);
    return true;
}

int32 UQuestSubsystem::GetCurrentStepIndexForQuest(FName QuestID, bool bAutoStartIfNotExists /* = false */)
{
    if (const int32* FoundStep = QuestProgress.Find(QuestID))
    {
        return *FoundStep;
    }
    if (bAutoStartIfNotExists)
    {
        if (TryStartQuest(QuestID))
        {
            return 0;
        }
    }
    return INDEX_NONE;
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
    int32 StepIndex = QuestProgress.FindChecked(QuestID);
    UE_LOG(LogEmber, Log, TEXT("[CompleteQuest] QuestID: %s, 마지막 StepIndex: %d"), *QuestID.ToString(), StepIndex);

    QuestProgress.Remove(QuestID);
    CompletedQuests.Add(QuestID);
    EMBER_LOG(LogEmber, Log, TEXT("Quest %s completed."), *QuestID.ToString());

    
    return true;
}
bool UQuestSubsystem::GetLastActiveQuestID(FName& OutQuestID) const
{
    if (LastAcceptedQuestID.IsNone())
    {
        return false;
    }

    OutQuestID = LastAcceptedQuestID;
    return true;
}
TMap<FName, TObjectPtr<UQuestDataAsset>>& UQuestSubsystem::GetAllLoadedQuests() 
{
    return LoadedQuests;
}

bool UQuestSubsystem::IsQuestAccepted(FName QuestID) const
{
    return QuestProgress.Contains(QuestID);
}
bool UQuestSubsystem::IsStepCompleted(FName QuestID, int32 StepIndex) const
{
    if (!QuestProgress.Contains(QuestID))
    {
        UE_LOG(LogTemp, Warning, TEXT("[IsStepCompleted] Quest %s: 수락되지 않음"), *QuestID.ToString());
        return false;
    }

    const int32 CurrentIndex = QuestProgress.FindChecked(QuestID);
    const bool bCompleted = StepIndex < CurrentIndex;

    UE_LOG(LogTemp, Warning, TEXT("[IsStepCompleted] Quest: %s | StepIndex: %d | CurrentIndex: %d | Completed: %d"),
        *QuestID.ToString(), StepIndex, CurrentIndex, bCompleted);

    return bCompleted;
}
