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

    /** ����Ʈ ���� �� ȣ��� */
    UFUNCTION()
    void HandleQuestStarted(UQuestDataAsset* QuestAsset);

    /** ���� ��ư Ŭ�� �� ȣ��� */
    UFUNCTION()
    void OnQuestEntryClicked(UQuestDataAsset* ClickedQuest, int32 StepIndex); 

    /** ����Ʈ�� �� ���ܸ��� ��ư ���� */
public:
    void AddQuest(UQuestDataAsset* QuestAsset, int32 StepIndex);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TSubclassOf<UQuestListButton> QuestEntryWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* QuestListScrollBox;

    /** ���õ� ����Ʈ ���� ������ ǥ���� ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    UPlayerQuestWidget* QuestContentsWidget;
};
