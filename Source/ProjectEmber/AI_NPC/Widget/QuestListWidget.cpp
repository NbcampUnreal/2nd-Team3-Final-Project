#include "QuestListWidget.h"
#include "Components/ScrollBox.h"
#include "Quest/QuestSubsystem.h"
#include "QuestListButton.h"
#include "Quest/Data/QuestDataAsset.h"
#include "PlayerQuestWidget.h"

void UQuestListWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (QuestContentsWidget)
    {
        QuestContentsWidget->SetVisibility(ESlateVisibility::Hidden); // ¶Ç´Â Hidden
    }

    if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
        Subsystem->OnQuestStarted.AddDynamic(this, &UQuestListWidget::HandleQuestStarted);
    }
}

void UQuestListWidget::HandleQuestStarted(UQuestDataAsset* QuestAsset)
{
    AddQuest(QuestAsset);
}

void UQuestListWidget::AddQuest(UQuestDataAsset* QuestAsset)
{
    if (!QuestAsset || !QuestEntryWidgetClass || !QuestListScrollBox) return;

    UQuestListButton* EntryWidget = CreateWidget<UQuestListButton>(this, QuestEntryWidgetClass);
    if (!EntryWidget) return;

    EntryWidget->SetQuestData(QuestAsset);
    EntryWidget->OnQuestItemClicked.AddDynamic(this, &UQuestListWidget::OnQuestEntryClicked);
    QuestListScrollBox->AddChild(EntryWidget);
}

void UQuestListWidget::OnQuestEntryClicked(UQuestDataAsset* ClickedQuest)
{
    if (!ClickedQuest || !QuestContentsWidget) return;

    if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
        const bool bIsAccepted = Subsystem->IsQuestAccepted(ClickedQuest->QuestID);
        const bool bIsComplete = Subsystem->IsQuestCompleted(ClickedQuest->QuestID);

        QuestContentsWidget->SetQuestInfoFromDataAsset(ClickedQuest, bIsComplete, bIsAccepted);
        QuestContentsWidget->SetVisibility(ESlateVisibility::Visible);
    }
}
