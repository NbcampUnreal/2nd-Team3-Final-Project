#include "QuestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Quest/Data/QuestDataAsset.h"
#include "Quest/QuestSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UQuestWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (AcceptButton)
    {
        AcceptButton->OnClicked.AddDynamic(this, &UQuestWidget::HandleAcceptClicked);
    }

    if (RefuseButton)
    {
        RefuseButton->OnClicked.AddDynamic(this, &UQuestWidget::HandleRefuseClicked);
    }

    if (CompleteButton)
    {
        CompleteButton->OnClicked.AddDynamic(this, &UQuestWidget::HandleCompleteClicked);
    }
}

void UQuestWidget::HandleAcceptClicked()
{
    UE_LOG(LogTemp, Warning, TEXT(">>> Click Accept Button"));
    OnQuestAccepted.Broadcast();
}

void UQuestWidget::HandleRefuseClicked()
{
    UE_LOG(LogTemp, Warning, TEXT(">>> Click Refused Button"));
    OnQuestRefused.Broadcast();
}

void UQuestWidget::HandleCompleteClicked()
{
    UE_LOG(LogTemp, Warning, TEXT(">>> Click Quest Complete Button"));
    OnQuestCompleted.Broadcast();
}

void UQuestWidget::SetQuestInfoFromDataAsset(const UQuestDataAsset* QuestAsset, bool bIsComplete, bool bIsAccepted)
{
    CurrentQuestAsset = QuestAsset;
    if (!QuestAsset) return;

    int32 StepIndexToUse = 0;

    UWorld* World = GetWorld();
    if (World)
    {
        if (UQuestSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
        {
            if (bIsAccepted && !bIsComplete)
            {
                int32 CurrentStepIndex = Subsystem->GetCurrentStepIndexForQuest(QuestAsset->QuestID);
                if (QuestAsset->Steps.IsValidIndex(CurrentStepIndex))
                {
                    StepIndexToUse = CurrentStepIndex;
                }
            }
            else if (bIsComplete)
            {
                StepIndexToUse = QuestAsset->Steps.Num() - 1;
            }
        }
    }

    if (!QuestAsset->Steps.IsValidIndex(StepIndexToUse)) return;
    const FQuestStep& Step = QuestAsset->Steps[StepIndexToUse];

    if (QuestNameText)
    {
        QuestNameText->SetText(Step.StepName.IsEmpty() ? QuestAsset->QuestName : Step.StepName);
    }

    if (QuestDescriptionText)
    {
        QuestDescriptionText->SetText(Step.StepQuestDescription.IsEmpty() ? QuestAsset->QuestDescription : Step.StepQuestDescription);
    }

    if (LocationText)
    {
        LocationText->SetText(Step.StepTargetLocationTag.IsEmpty() ? QuestAsset->TargetLocationTag : Step.StepTargetLocationTag);
    }

    if (RewardText)
    {
        int32 Gold = 0, Exp = 0;
        for (const FQuestRewardData& Reward : Step.Rewards)
        {
            if (Reward.RewardTag == FGameplayTag::RequestGameplayTag("Reward.Gold"))
                Gold += Reward.RewardCount;
            else if (Reward.RewardTag == FGameplayTag::RequestGameplayTag("Reward.Exp"))
                Exp += Reward.RewardCount;
        }

        FString RewardSummary = FString::Printf(TEXT("Gold: %d / Exp: %d"), Gold, Exp);
        RewardText->SetText(FText::FromString(RewardSummary));
    }

}