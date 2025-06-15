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

    //  위젯 생성되면 무조건 숨김 상태로 시작!
    SetVisibility(ESlateVisibility::Hidden);
  
}

void UQuestTracker::ShowTracker(FText NewStepType, FText NewStepName, UQuestCondition* Condition, bool bIsComplete)
{
    UTextBlock* CompleteText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Complete")));
    UTextBlock* StepTypeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Step_Type")));
    UTextBlock* StepNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Step_Name")));

    //   무조건 CompleteText는 초기 상태에서 숨김으로 시작!
    if (CompleteText)
    {
        CompleteText->SetVisibility(ESlateVisibility::Hidden);
        CompleteText->SetText(FText::GetEmpty());
    }

    //   StepType / StepName은 항상 표시
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

    //  Condition 따라 아이콘 표시 (동일)
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

    //   Complete 모드라면 CompleteText 추가로 표시 + 텍스트 설정
    if (bIsComplete && CompleteText)
    {
        CompleteText->SetVisibility(ESlateVisibility::Visible);
        CompleteText->SetText(FText::FromString(TEXT("Complete")));
    }

    //  트래커 보이기
    SetVisibility(ESlateVisibility::Visible);

    //   3초 후 자동 숨김
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            SetVisibility(ESlateVisibility::Hidden);
        }, 3.0f, false);
}