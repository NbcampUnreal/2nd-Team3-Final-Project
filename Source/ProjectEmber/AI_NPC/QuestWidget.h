// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestDataRow.h"
#include "QuestWidget.generated.h"


class UButton;
class UDataTable;
class UTextBlock;

DECLARE_DELEGATE(FOnQuestAccepted)
DECLARE_DELEGATE(FOnQuestRefused)

UCLASS()
class PROJECTEMBER_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:


    virtual void NativeConstruct() override;

    void SetQuestInfoFromDataRow(const FQuestDataRow& Data);

    FOnQuestAccepted OnQuestAccepted;
    FOnQuestRefused OnQuestRefused;

    UPROPERTY(BlueprintReadWrite, Category = "QuestData")
    FName QuestRowName;

    UPROPERTY(BlueprintReadWrite, Category = "QuestData")
    UDataTable* QuestDataTable;

protected:
    UFUNCTION()
    void HandleAcceptClicked();

    UFUNCTION()
    void HandleRefuseClicked();

    /** 바인딩된 위젯들 */
    UPROPERTY(meta = (BindWidget))
    UButton* AcceptButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RefuseButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestDescriptionText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ObjectiveText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RewardText;
};