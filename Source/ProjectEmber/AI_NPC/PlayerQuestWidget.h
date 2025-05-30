// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestDataRow.h"
#include "Components/Border.h"
#include "PlayerQuestWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECTEMBER_API UPlayerQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetQuestInfoFromDataRow(const FQuestDataRow& QuestRow, bool bIsComplete = false);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuestNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuestDescriptionText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ObjectiveNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LocationText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RewardText;

	UPROPERTY(meta = (BindWidget))
	UBorder* Border_512;
};