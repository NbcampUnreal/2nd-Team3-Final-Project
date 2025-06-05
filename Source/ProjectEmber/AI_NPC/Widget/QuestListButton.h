// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestListButton.generated.h"

class UButton;
class UTextBlock;
class UQuestDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestItemClicked, UQuestDataAsset*, ClickedQuest);

UCLASS()
class PROJECTEMBER_API UQuestListButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(meta = (BindWidget))
    UButton* RootButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestNameText;

    /** ���ε��� ����Ʈ ������ */
    UPROPERTY()
    UQuestDataAsset* QuestAsset;

    /** �ܺο��� Ŭ���� ���� �� �ְ� ��������Ʈ ���� */
    UPROPERTY(BlueprintAssignable)
    FOnQuestItemClicked OnQuestItemClicked;

    /** ����Ʈ ������ �ܺο��� ���� */
    void SetQuestData(UQuestDataAsset* InAsset);

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void HandleClick();
};