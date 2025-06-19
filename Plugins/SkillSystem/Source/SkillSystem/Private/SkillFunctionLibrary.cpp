#include "SkillFunctionLibrary.h"
#include "SkillManagerSubsystem.h"

USkillManagerSubsystem* USkillFunctionLibrary::GetSkillManager()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetGameInstance())
	{
		return GEngine->GameViewport->GetGameInstance()->GetSubsystem<USkillManagerSubsystem>();
	}
	
	return nullptr; 
}
