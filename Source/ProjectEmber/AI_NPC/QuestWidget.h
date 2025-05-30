// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest/Data/QuestDataAsset.h"
#include "QuestWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnQuestAccepted);
DECLARE_MULTICAST_DELEGATE(FOnQuestRefused);
DECLARE_MULTICAST_DELEGATE(FOnQuestCompleted);

class UButton;
class UDataTable;
class UTextBlock;


UCLASS()
class PROJECTEMBER_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void SetQuestInfoFromDataAsset(const UQuestDataAsset* QuestAsset, bool bIsComplete, bool bIsAccepted);

    FOnQuestAccepted OnQuestAccepted;
    FOnQuestRefused OnQuestRefused;
    FOnQuestCompleted OnQuestCompleted;

    const UQuestDataAsset* CurrentQuestAsset = nullptr;

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

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void HandleAcceptClicked();

    UFUNCTION()
    void HandleRefuseClicked();

    UFUNCTION()
    void HandleCompleteClicked();
};