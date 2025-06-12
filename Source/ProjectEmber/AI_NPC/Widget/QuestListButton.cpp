#include "QuestListButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Quest/Data/QuestDataAsset.h"

void UQuestListButton::NativeConstruct()
{
    Super::NativeConstruct();

    if (RootButton)
    {
        RootButton->OnClicked.AddDynamic(this, &UQuestListButton::HandleClick);
    }
}

void UQuestListButton::SetQuestData(UQuestDataAsset* InQuestAsset, int32 InStepIndex)
{
    QuestAsset = InQuestAsset;
    StepIndex = InStepIndex;

    if (QuestNameText && QuestAsset && QuestAsset->Steps.IsValidIndex(StepIndex))
    {
        const FText StepName = QuestAsset->Steps[StepIndex].StepQuestListName;
        QuestNameText->SetText(StepName);
    }
    else if (QuestNameText)
    {
        QuestNameText->SetText(FText::FromString(TEXT("Unknown Step")));
    }
}

void UQuestListButton::HandleClick()
{
    if (QuestAsset)
    {
        OnQuestItemClicked.Broadcast(QuestAsset, StepIndex); // 저장된 인덱스 전달
    }
}