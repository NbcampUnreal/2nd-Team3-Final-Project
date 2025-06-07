// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestListButton.generated.h"

class UButton;
class UTextBlock;
class UQuestDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestItemClicked, UQuestDataAsset*, QuestAsset, int32, StepIndex);


UCLASS()
class PROJECTEMBER_API UQuestListButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    void SetQuestData(UQuestDataAsset* InQuestAsset, int32 InStepIndex);

    UPROPERTY(BlueprintAssignable)
    FOnQuestItemClicked OnQuestItemClicked;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* RootButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestNameText;

    UQuestDataAsset* QuestAsset;
    int32 StepIndex = 0;

private:
    UFUNCTION()
    void HandleClick();
};