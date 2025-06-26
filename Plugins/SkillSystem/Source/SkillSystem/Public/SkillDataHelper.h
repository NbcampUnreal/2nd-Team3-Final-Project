// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillTreeDataAsset.h"
#include "UObject/Object.h"
#include "SkillDataHelper.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SKILLSYSTEM_API USkillDataHelper : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<USkillTreeDataAsset> SkillTreeData{nullptr};
};
