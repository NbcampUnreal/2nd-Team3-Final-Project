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
    RemoveFromParent(); // ����Ʈ UI �ݱ�

    // �ܺο� �˸�
    if (OnQuestAccepted.IsBound())
    {
        OnQuestAccepted.Execute();
    }
}

void UQuestWidget::HandleRefuseClicked()
{
    RemoveFromParent(); // ����Ʈ UI �ݱ�

    // �ܺο� �˸�
    if (OnQuestRefused.IsBound())
    {
        OnQuestRefused.Execute();
    }
}