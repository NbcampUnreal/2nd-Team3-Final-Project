#include "PlayerQuestWidget.h"
#include "Components/TextBlock.h"

void UPlayerQuestWidget::SetQuestInfoFromDataRow(const FQuestDataRow& QuestRow, bool bIsComplete)
{
	FString DisplayName = QuestRow.QuestName;
	if (bIsComplete)
	{
		DisplayName += TEXT(" (Complete)");
		if (Border_512)
		{
			Border_512->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (Border_512)
		{
			Border_512->SetVisibility(ESlateVisibility::Collapsed);
		}
	
	}
	if (QuestNameText)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestNameText 설정 중: %s"), *DisplayName);
		QuestNameText->SetText(FText::FromString(DisplayName));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("QuestNameText is NULL! 이름이 안 맞는 듯 합니다."));
	}

	if (QuestDescriptionText)
	{
		QuestDescriptionText->SetText(FText::FromString(QuestRow.Description));
	}

	if (ObjectiveNameText)
	{
		FString CombinedObjectives;
		for (int32 i = 0; i < QuestRow.ObjectiveNames.Num(); ++i)
		{
			FString ObjectiveLine = QuestRow.ObjectiveNames[i];
			int32 Goal = QuestRow.ObjectiveGoals.IsValidIndex(i) ? QuestRow.ObjectiveGoals[i] : 1;
			CombinedObjectives += FString::Printf(TEXT("- %s (%d)\n"), *ObjectiveLine, Goal);
		}
		ObjectiveNameText->SetText(FText::FromString(CombinedObjectives));
	}

	if (LocationText)
	{
		LocationText->SetText(FText::FromString(QuestRow.TargetLocation));
	}

	if (RewardText)
	{
		FString RewardStr = FString::Printf(TEXT("%d Gold / %d EXP"), QuestRow.RewardGold, QuestRow.RewardExp);
		RewardText->SetText(FText::FromString(RewardStr));
	}
}