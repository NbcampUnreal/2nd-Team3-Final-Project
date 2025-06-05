#include "PlayerQuestWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Quest/Data/QuestDataAsset.h"
#include "Quest/QuestSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UPlayerQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Border_512)
	{
		Border_512->SetVisibility(ESlateVisibility::Collapsed); // 기본은 숨김
	}
}


void UPlayerQuestWidget::SetQuestInfoFromDataAsset(const UQuestDataAsset* QuestAsset, bool bIsComplete, bool bIsAccepted)
{
	if (!QuestAsset) return;

	// 미션 완료 보더 표시
	if (Border_512)
	{
		Border_512->SetVisibility(bIsComplete ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// 수락 전: 전체 퀘스트 정보 표시
	if (!bIsAccepted)
	{
		if (QuestNameText) QuestNameText->SetText(QuestAsset->QuestName);
		if (QuestDescriptionText) QuestDescriptionText->SetText(QuestAsset->QuestDescription);
		if (LocationText) LocationText->SetText(QuestAsset->TargetLocationTag);

		if (RewardText)
		{
			int32 TotalGold = 0, TotalExp = 0;
			for (const FQuestStep& Step : QuestAsset->Steps)
			{
				for (const FQuestRewardData& Reward : Step.Rewards)
				{
					if (Reward.RewardTag == FGameplayTag::RequestGameplayTag("Reward.Gold"))
						TotalGold += Reward.RewardCount;
					else if (Reward.RewardTag == FGameplayTag::RequestGameplayTag("Reward.Exp"))
						TotalExp += Reward.RewardCount;
				}
			}
			FString RewardStr = FString::Printf(TEXT("%d Gold / %d EXP"), TotalGold, TotalExp);
			RewardText->SetText(FText::FromString(RewardStr));
		}
		if (ObjectiveNameText)
		{
			FString Objectives;
			for (const FQuestStep& Step : QuestAsset->Steps)
			{
				FString Line = Step.ObjectiveName.IsEmpty()
					? Step.StepID.ToString()
					: Step.ObjectiveName.ToString();

				Objectives += Line + TEXT("\n");
			}
			ObjectiveNameText->SetText(FText::FromString(Objectives));
		}

		return;
	}

	// 수락 후: 현재 Step만 표시
	if (UWorld* World = GetWorld())
	{
		if (UQuestSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
		{
			int32 StepIndex = Subsystem->GetCurrentStepIndexForQuest(QuestAsset->QuestID);
			if (!QuestAsset->Steps.IsValidIndex(StepIndex)) return;

			const FQuestStep& Step = QuestAsset->Steps[StepIndex];

			if (QuestNameText) QuestNameText->SetText(Step.StepName.IsEmpty() ? QuestAsset->QuestName : Step.StepName);
			if (QuestDescriptionText) QuestDescriptionText->SetText(Step.StepQuestDescription.IsEmpty() ? QuestAsset->QuestDescription : Step.StepQuestDescription);
			if (LocationText) LocationText->SetText(Step.StepTargetLocationTag.IsEmpty() ? QuestAsset->TargetLocationTag : Step.StepTargetLocationTag);

			if (RewardText)
			{
				int32 Gold = 0, Exp = 0;
				for (const FQuestRewardData& Reward : Step.Rewards)
				{
					if (Reward.RewardTag == FGameplayTag::RequestGameplayTag("Reward.Gold"))
						Gold += Reward.RewardCount;
					else if (Reward.RewardTag == FGameplayTag::RequestGameplayTag("Reward.Exp"))
						Exp += Reward.RewardCount;
				}
				FString RewardStr = FString::Printf(TEXT("%d Gold / %d EXP"), Gold, Exp);
				RewardText->SetText(FText::FromString(RewardStr));
			}

			if (ObjectiveNameText)
			{
				ObjectiveNameText->SetText(Step.ObjectiveName.IsEmpty()
					? FText::FromString(Step.StepID.ToString())
					: Step.ObjectiveName);
			}
		}
	}
}
