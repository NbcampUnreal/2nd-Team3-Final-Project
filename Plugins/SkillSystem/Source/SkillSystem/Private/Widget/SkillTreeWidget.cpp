#include "Widget/SkillTreeWidget.h"
#include "SkillManagerSubsystem.h"

void USkillTreeWidget::SetDetailInfo(FName SkillID)
{
	USkillManagerSubsystem* SkillManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USkillManagerSubsystem>();
	
	FSkillDetailInfo DetailInfo = SkillManagerSubsystem->GetDetail(SkillID);

	bool bCanUnlock = SkillManagerSubsystem->CanUnlockSkill(SkillID);
	bool bIsUnlocked = SkillManagerSubsystem->IsSkillUnlocked(SkillID);
	SkillDetailWidget->SetupDetail(SkillID, DetailInfo.DisplayName, DetailInfo.Description, bCanUnlock, bIsUnlocked);
}
