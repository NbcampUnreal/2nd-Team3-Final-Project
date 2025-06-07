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
        QuestContentsWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
        Subsystem->OnQuestStarted.AddDynamic(this, &UQuestListWidget::HandleQuestStarted);
    }
}

void UQuestListWidget::HandleQuestStarted(UQuestDataAsset* QuestAsset)
{
    if (!QuestAsset) return;

    // 현재 퀘스트의 진행 중인 스텝만 버튼으로 추가
    if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
        int32 StepIndex = Subsystem->GetCurrentStepIndexForQuest(QuestAsset->QuestID);
        AddQuest(QuestAsset, StepIndex);
    }
}

void UQuestListWidget::AddQuest(UQuestDataAsset* QuestAsset, int32 StepIndex)
{
    if (!QuestAsset || !QuestEntryWidgetClass || !QuestListScrollBox) return;

    if (!QuestAsset->Steps.IsValidIndex(StepIndex)) return;

    UQuestListButton* EntryWidget = CreateWidget<UQuestListButton>(this, QuestEntryWidgetClass);
    if (!EntryWidget) return;

    EntryWidget->SetQuestData(QuestAsset, StepIndex);
    EntryWidget->OnQuestItemClicked.AddDynamic(this, &UQuestListWidget::OnQuestEntryClicked);
    QuestListScrollBox->AddChild(EntryWidget);
}

void UQuestListWidget::OnQuestEntryClicked(UQuestDataAsset* ClickedQuest, int32 StepIndex)
{
    if (!ClickedQuest || !QuestContentsWidget) return;

    if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
        const bool bIsAccepted = Subsystem->IsQuestAccepted(ClickedQuest->QuestID);
        const bool bIsComplete = Subsystem->IsQuestCompleted(ClickedQuest->QuestID);
        const bool bShowStepComplete = Subsystem->IsStepCompleted(ClickedQuest->QuestID, StepIndex);

        QuestContentsWidget->SetQuestInfoFromDataAsset(ClickedQuest, bIsComplete, bIsAccepted, bShowStepComplete, StepIndex);
        QuestContentsWidget->SetVisibility(ESlateVisibility::Visible);
    }
}