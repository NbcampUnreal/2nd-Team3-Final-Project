#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpec.h"
#include "SkillTreeDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FSkillDetailInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Detail")
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Detail")
	FText Description;
};

USTRUCT(BlueprintType)
struct FSkillNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FName SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FSkillDetailInfo FSkillDetailInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TSubclassOf<UGameplayAbility> AbilityClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TArray<FName> NextSkillIDs;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TArray<FName> PrerequisiteSkillIDs;
};

UCLASS(BlueprintType)
class SKILLSYSTEM_API USkillTreeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree")
	TArray<FSkillNode> SkillNodes;
};
