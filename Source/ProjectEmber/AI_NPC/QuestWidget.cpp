#include "QuestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

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
    RemoveFromParent();
    if (OnQuestAccepted.IsBound())
    {
        OnQuestAccepted.Execute();
    }
}

void UQuestWidget::HandleRefuseClicked()
{
    RemoveFromParent();
    if (OnQuestRefused.IsBound())
    {
        OnQuestRefused.Execute();
    }
}

void UQuestWidget::HandleCompleteClicked()
{
    RemoveFromParent();
    if (OnQuestCompleted.IsBound())
    {
        OnQuestCompleted.Execute();
    }
}

void UQuestWidget::SetQuestInfoFromDataRow(const FQuestDataRow& Data)
{
    if (QuestNameText)
    {
        QuestNameText->SetText(FText::FromString(Data.QuestName));
    }

    if (QuestDescriptionText)
    {
        QuestDescriptionText->SetText(FText::FromString(Data.Description));
    }

    if (LocationText)
    {
        FString LocationInfo = Data.TargetLocation.IsEmpty() ? TEXT("알 수 없음") : Data.TargetLocation;
        LocationText->SetText(FText::FromString(LocationInfo));
    }

    if (RewardText)
    {
        FString RewardSummary = FString::Printf(TEXT("Gold: %d / Exp: %d"), Data.RewardGold, Data.RewardExp);
        RewardText->SetText(FText::FromString(RewardSummary));
    }
}