// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestListWidget.generated.h"

class UScrollBox;
class UQuestListButton;
class UQuestDataAsset;
class UPlayerQuestWidget;

UCLASS()
class PROJECTEMBER_API UQuestListWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

    /** ��������Ʈ ���ſ� */
    UFUNCTION()
    void HandleQuestStarted(UQuestDataAsset* QuestAsset);

    /** ��ư Ŭ�� �� �� ���� ��� */
    UFUNCTION()
    void OnQuestEntryClicked(UQuestDataAsset* ClickedQuest);

    /** ��ư ���� */
    void AddQuest(UQuestDataAsset* QuestAsset);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TSubclassOf<UQuestListButton> QuestEntryWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* QuestListScrollBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    UPlayerQuestWidget* QuestContentsWidget;
};
