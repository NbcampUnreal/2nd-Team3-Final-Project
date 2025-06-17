#include "Widget/SkillDetailWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USkillDetailWidget::SetupDetail(const FName& InSkillID, FText InName, FText InDesc, bool bCanUnlock, bool bUnlocked)
{
	SkillID = InSkillID;
	
	if (SkillNameText)
	{
		SkillNameText->SetText(InName);
	}
	
	if (DetailText)
	{
		DetailText->SetText(InDesc);
	}
	
	if (UnlockButton)
	{
		UnlockButton->SetIsEnabled(!bUnlocked && bCanUnlock);

		if (bUnlocked)
		{
			static FString UnlockString = FString::Printf(TEXT("Unlocked"));
			UnlockText->SetText(FText::FromString(UnlockString));
		}
		else
		{
			static FString UnlockString = FString::Printf(TEXT("Unlock"));
			UnlockText->SetText(FText::FromString(UnlockString));
		}
	}
}
