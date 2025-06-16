#include "QuestTracker.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/Image.h"
#include "Quest/Condition/Dialogue/DialogueQuestCondition.h"
#include "Quest/Condition/Kill/QuestConditionKillAnimal.h"
#include "Quest/Condition/Gathering/QuestConditiongathering.h"


void UQuestTracker::NativeConstruct()
{
    Super::NativeConstruct();

    //  ���� �����Ǹ� ������ ���� ���·� ����!
    SetVisibility(ESlateVisibility::Hidden);
  
}

void UQuestTracker::ShowTracker(FText NewStepType, FText NewStepName, UQuestCondition* Condition, bool bIsComplete)
{
    UTextBlock* CompleteText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Complete")));
    UTextBlock* StepTypeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Step_Type")));
    UTextBlock* StepNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Step_Name")));

    //   ������ CompleteText�� �ʱ� ���¿��� �������� ����!
    if (CompleteText)
    {
        CompleteText->SetVisibility(ESlateVisibility::Hidden);
        CompleteText->SetText(FText::GetEmpty());
    }

    //   StepType / StepName�� �׻� ǥ��
    if (StepTypeText)
    {
        StepTypeText->SetVisibility(ESlateVisibility::Visible);
        StepTypeText->SetText(NewStepType);
    }

    if (StepNameText)
    {
        StepNameText->SetVisibility(ESlateVisibility::Visible);
        StepNameText->SetText(NewStepName);
    }

    //  Condition ���� ������ ǥ�� (����)
    UImage* DialogueImg = Cast<UImage>(GetWidgetFromName(TEXT("DialogueImage")));
    UImage* HuntingImg = Cast<UImage>(GetWidgetFromName(TEXT("HuntingImage")));
    UImage* CollectingImg = Cast<UImage>(GetWidgetFromName(TEXT("CollectingImage")));
    UImage* FightingImg = Cast<UImage>(GetWidgetFromName(TEXT("Fighting_Image")));
    UImage* CoinImg = Cast<UImage>(GetWidgetFromName(TEXT("Coin_Image")));
    UImage* CircleImg = Cast<UImage>(GetWidgetFromName(TEXT("Circle_Image")));
    UImage* AllowImg = Cast<UImage>(GetWidgetFromName(TEXT("Allow_Image")));

    if (DialogueImg) DialogueImg->SetVisibility(ESlateVisibility::Hidden);
    if (HuntingImg) HuntingImg->SetVisibility(ESlateVisibility::Hidden);
    if (CollectingImg) CollectingImg->SetVisibility(ESlateVisibility::Hidden);
    if (FightingImg) FightingImg->SetVisibility(ESlateVisibility::Hidden);
    if (CoinImg) CoinImg->SetVisibility(ESlateVisibility::Hidden);
    if (CircleImg) CircleImg->SetVisibility(ESlateVisibility::Hidden);
    if (AllowImg) AllowImg->SetVisibility(ESlateVisibility::Hidden);

    if (Condition)
    {
        if (Condition->IsA(UDialogueQuestCondition::StaticClass()))
        {
            if (DialogueImg) DialogueImg->SetVisibility(ESlateVisibility::Visible);
        }
        else if (Condition->IsA(UQuestConditionKillAnimal::StaticClass()))
        {
            if (HuntingImg) HuntingImg->SetVisibility(ESlateVisibility::Visible);
        }
        else if (Condition->IsA(UQuestConditiongathering::StaticClass()))
        {
            if (CollectingImg) CollectingImg->SetVisibility(ESlateVisibility::Visible);
        }
    }

    //   Complete ����� CompleteText �߰��� ǥ�� + �ؽ�Ʈ ����
    if (bIsComplete && CompleteText)
    {
        CompleteText->SetVisibility(ESlateVisibility::Visible);
        CompleteText->SetText(FText::FromString(TEXT("Complete")));
    }

    //  Ʈ��Ŀ ���̱�
    SetVisibility(ESlateVisibility::Visible);

    //   3�� �� �ڵ� ����
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            SetVisibility(ESlateVisibility::Hidden);
        }, 3.0f, false);
}