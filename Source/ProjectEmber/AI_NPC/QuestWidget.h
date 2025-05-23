// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestDataRow.h"
#include "QuestWidget.generated.h"


class UButton;
class UDataTable;
class UTextBlock;

DECLARE_DELEGATE(FOnQuestAccepted);
DECLARE_DELEGATE(FOnQuestRefused);
DECLARE_DELEGATE(FOnQuestCompleted);

UCLASS()
class PROJECTEMBER_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;
    void SetQuestInfoFromDataRow(const FQuestDataRow& Data);

    FOnQuestAccepted OnQuestAccepted;
    FOnQuestRefused OnQuestRefused;
    FOnQuestCompleted OnQuestCompleted;

    UPROPERTY(meta = (BindWidgetOptional))
    class UButton* AcceptButton;

    UPROPERTY(meta = (BindWidgetOptional))
    class UButton* RefuseButton;

    UPROPERTY(meta = (BindWidgetOptional))
    class UButton* CompleteButton;

    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* QuestNameText;

    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* QuestDescriptionText;

    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* LocationText;

    UPROPERTY(meta = (BindWidgetOptional))
    class UTextBlock* RewardText;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    UDataTable* QuestDataTable;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    FName QuestRowName;

private:
    UFUNCTION()
    void HandleAcceptClicked();

    UFUNCTION()
    void HandleRefuseClicked();

    UFUNCTION()
    void HandleCompleteClicked();
};