// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillDetailWidget.h"
#include "Blueprint/UserWidget.h"
#include "SkillTreeWidget.generated.h"

/**
 * 
 */
UCLASS()
class SKILLSYSTEM_API USkillTreeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void SetDetailInfo(FName SkillID);

	UFUNCTION(BlueprintImplementableEvent, Category="Skill")
	void ShowDetailWidget();
	
	UPROPERTY(meta = (BindWidget))
	USkillDetailWidget* SkillDetailWidget;
};
