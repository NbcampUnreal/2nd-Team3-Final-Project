#include "QuestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "QuestDataRow.h"

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
}

void UQuestWidget::HandleAcceptClicked()
{
    RemoveFromParent(); // Äù½ºÆ® UI ´Ý±â

    if (OnQuestAccepted.IsBound())
    {
        OnQuestAccepted.Execute();
    }
}

void UQuestWidget::HandleRefuseClicked()
{
    RemoveFromParent(); // Äù½ºÆ® UI ´Ý±â

    if (OnQuestRefused.IsBound())
    {
        OnQuestRefused.Execute();
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

    if (ObjectiveText)
    {
        FString ObjectiveSummary;
        for (const FString& Obj : Data.ObjectiveNames)
        {
            ObjectiveSummary += Obj + TEXT("\n");
        }
        ObjectiveText->SetText(FText::FromString(ObjectiveSummary));
    }

    if (RewardText)
    {
        FString RewardSummary = FString::Printf(TEXT("Gold: %d / Exp: %d"), Data.RewardGold, Data.RewardExp);
        RewardText->SetText(FText::FromString(RewardSummary));
    }
}