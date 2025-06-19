#include "SkillManagerSubsystem.h"

#include "EMSFunctionLibrary.h"
#include "Engine/AssetManager.h"
#include "Abilities/GameplayAbility.h"

USkillManagerSubsystem* USkillManagerSubsystem::GetSkillManagerSubsystem(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(
	WorldContextObject, EGetWorldErrorMode::LogAndReturnNull
);
	if (!World) return nullptr;

	// 해당 World의 GameInstance → Subsystem 반환
	UGameInstance* GI = World->GetGameInstance();
	return GI ? GI->GetSubsystem<USkillManagerSubsystem>() : nullptr;
}

void USkillManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// 기본 경로에서 DataAsset 로드 (프로젝트에 맞게 경로 수정)
	SkillTreeData = LoadObject<USkillTreeDataAsset>(nullptr, TEXT("/Script/SkillSystem.SkillTreeDataAsset'/Game/_Blueprints/UI/GameMenu/Skill/DA_SkillTree.DA_SkillTree'"));
}

const FSkillNode* USkillManagerSubsystem::FindSkillNode(FName SkillID) const
{
	if (!SkillTreeData) return nullptr;
	for (const FSkillNode& Node : SkillTreeData->SkillNodes)
	{
		if (Node.SkillID == SkillID)
			return &Node;
	}
	return nullptr;
}

const FSkillNode* USkillManagerSubsystem::GetSkillNode(const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	if (!SkillTreeData) return nullptr;
	for (const FSkillNode& Node : SkillTreeData->SkillNodes)
	{
		if (Node.AbilityClass == AbilityClass)
			return &Node;
	}
	return nullptr;
}

void USkillManagerSubsystem::UnlockSkill(FName SkillID)
{
	if (!CanUnlockSkill(SkillID))
	{
		UE_LOG(LogTemp, Warning, TEXT("UnlockSkill failed: prerequisites not met for %s."), *SkillID.ToString());
		return;
	}
	UnlockedSkills.Add(SkillID);
	UE_LOG(LogTemp, Log, TEXT("Skill unlocked: %s."), *SkillID.ToString());
	
	// TODO: SaveGame 처리 로직 추가
}

bool USkillManagerSubsystem::CanUnlockSkill(FName SkillID) const
{
	const FSkillNode* Node = FindSkillNode(SkillID);
	if (!Node)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanUnlockSkill: SkillID %s not found."), *SkillID.ToString());
		return false;
	}
	
	for (const FName& PreID : Node->PrerequisiteSkillIDs)
	{
		if (!UnlockedSkills.Contains(PreID))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot unlock %s, missing prerequisite %s."), *SkillID.ToString(), *PreID.ToString());
			return false;
		}
	}
	return true;
}

bool USkillManagerSubsystem::IsSkillUnlocked(FName SkillID) const
{
	return UnlockedSkills.Contains(SkillID);
}

TArray<TSubclassOf<UGameplayAbility>> USkillManagerSubsystem::GetNextComboAbilities(const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	TArray<TSubclassOf<UGameplayAbility>> Result;
	const FSkillNode* Node = GetSkillNode(AbilityClass);
	if (Node)
	{
		for (const FName& NextID : Node->NextSkillIDs)
		{
			if (IsSkillUnlocked(NextID))
			{
				const FSkillNode* NextNode = FindSkillNode(NextID);
				if (NextNode && NextNode->AbilityClass)
					Result.Add(NextNode->AbilityClass);
			}
		}
	}
	return Result;
}

const FSkillNode* USkillManagerSubsystem::GetSkillNode(FName SkillID) const
{
	return FindSkillNode(SkillID);
}

FSkillDetailInfo USkillManagerSubsystem::GetDetail(FName SkillID)
{
	if (const FSkillNode* SkillNode = FindSkillNode(SkillID))
	{
		return SkillNode->FSkillDetailInfo;
	}
	
	return FSkillDetailInfo();
}
