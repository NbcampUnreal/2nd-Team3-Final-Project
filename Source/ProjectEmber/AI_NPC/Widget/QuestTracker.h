// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest/Condition/QuestCondition.h"
#include "QuestTracker.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UQuestTracker : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void ShowTracker(FText NewStepType, FText NewStepName, UQuestCondition* Condition, bool bIsComplete);

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* DialogueImage;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* HuntingImage;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CollectingImage;
};