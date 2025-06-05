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

void UQuestListButton::SetQuestData(UQuestDataAsset* InAsset)
{
    QuestAsset = InAsset;

    if (QuestNameText && QuestAsset && QuestAsset->Steps.IsValidIndex(0))
    {
        QuestNameText->SetText(QuestAsset->Steps[0].StepQuestListName);
    }
}

void UQuestListButton::HandleClick()
{
    if (QuestAsset)
    {
        OnQuestItemClicked.Broadcast(QuestAsset);
    }
}
