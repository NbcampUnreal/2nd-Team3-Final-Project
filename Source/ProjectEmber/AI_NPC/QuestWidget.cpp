#include "QuestWidget.h"
#include "Components/Button.h"

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
    RemoveFromParent(); // 퀘스트 UI 닫기

    // 외부에 알림
    if (OnQuestAccepted.IsBound())
    {
        OnQuestAccepted.Execute();
    }
}

void UQuestWidget::HandleRefuseClicked()
{
    RemoveFromParent(); // 퀘스트 UI 닫기

    // 외부에 알림
    if (OnQuestRefused.IsBound())
    {
        OnQuestRefused.Execute();
    }
}