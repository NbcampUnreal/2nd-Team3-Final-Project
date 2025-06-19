// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillDetailWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class SKILLSYSTEM_API USkillDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Skill")
	void SetupDetail(const FName& InSkillID, FText InName, FText InDesc, bool bCanUnlock, bool bUnlocked);
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillNameText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DetailText;
	UPROPERTY(meta = (BindWidget))
	UButton* UnlockButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* UnlockText;
	
	UPROPERTY(BlueprintReadWrite, Category="Skill")
	FName SkillID;
	
};
