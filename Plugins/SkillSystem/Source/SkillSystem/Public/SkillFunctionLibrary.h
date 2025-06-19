#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SkillFunctionLibrary.generated.h"

class USkillManagerSubsystem;
/**
 * 
 */
UCLASS()
class SKILLSYSTEM_API USkillFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	static USkillManagerSubsystem* GetSkillManager();
};
