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

    /** 바인딩용 퀘스트 데이터 */
    UPROPERTY()
    UQuestDataAsset* QuestAsset;

    /** 외부에서 클릭을 받을 수 있게 델리게이트 제공 */
    UPROPERTY(BlueprintAssignable)
    FOnQuestItemClicked OnQuestItemClicked;

    /** 퀘스트 정보를 외부에서 세팅 */
    void SetQuestData(UQuestDataAsset* InAsset);

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void HandleClick();
};