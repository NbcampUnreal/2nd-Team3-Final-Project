#pragma once

#include "CoreMinimal.h"
#include "EMSActorSaveInterface.h"
#include "SkillDataHelper.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkillTreeDataAsset.h"
#include "SkillManagerSubsystem.generated.h"


UCLASS(Config = Engine, DefaultConfig, Blueprintable)
class SKILLSYSTEM_API USkillManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category="Skill", meta=(WorldContext="WorldContextObject"))
	static USkillManagerSubsystem* GetSkillManagerSubsystem(UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION()
	void OnAllActorsLoaded();
public:
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void UnlockSkill(FName SkillID);
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool CanUnlockSkill(FName SkillID) const;
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool IsSkillUnlocked(FName SkillID) const;
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	TArray<TSubclassOf<UGameplayAbility>> GetNextComboAbilities(const TSubclassOf<UGameplayAbility>& AbilityClass) const;

	const FSkillNode* GetSkillNode(FName SkillID) const;
	const FSkillNode* GetSkillNode(const TSubclassOf<UGameplayAbility>& AbilityClass) const;
	FSkillDetailInfo GetDetail(FName SkillID);

private:
	UPROPERTY(SaveGame)
	TSet<FName> UnlockedSkills;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USkillTreeDataAsset* SkillTreeData{nullptr};

	UPROPERTY(Config)
	TSubclassOf<USkillDataHelper> SkillTreeDataHelperClass;

	UPROPERTY()
	USkillDataHelper* SkillTreeDataHelperInstance;
	
	const FSkillNode* FindSkillNode(FName SkillID) const;
};
