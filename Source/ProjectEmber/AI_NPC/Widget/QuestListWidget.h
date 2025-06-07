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

    /** 퀘스트 시작 시 호출됨 */
    UFUNCTION()
    void HandleQuestStarted(UQuestDataAsset* QuestAsset);

    /** 스텝 버튼 클릭 시 호출됨 */
    UFUNCTION()
    void OnQuestEntryClicked(UQuestDataAsset* ClickedQuest, int32 StepIndex); 

    /** 퀘스트의 각 스텝마다 버튼 생성 */
    void AddQuest(UQuestDataAsset* QuestAsset, int32 StepIndex);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TSubclassOf<UQuestListButton> QuestEntryWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* QuestListScrollBox;

    /** 선택된 퀘스트 스텝 내용을 표시할 위젯 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    UPlayerQuestWidget* QuestContentsWidget;
};
