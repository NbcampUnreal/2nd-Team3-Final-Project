// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMenuWidget.generated.h"

class UWidgetSwitcher;
class UUserWidget; 
class UPlayerQuestWidget;

UCLASS()
class PROJECTEMBER_API UGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="UI|Layer")
	void Input_ToggleInventory();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="UI|Layer")
	void Input_ToggleQuest();

    /** Quest 위젯 안의 QuestContents를 찾아 업데이트하는 함수 */
    UFUNCTION(BlueprintCallable)
    void UpdateQuestInfoViaWidgetTree();

protected:

    /** 퀘스트 전환용 위젯 스위처 */
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UWidgetSwitcher* WidgetSwitcher;

    /** 중간 퀘스트 래퍼 위젯 (블루프린트: WBP_Quest) */
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UUserWidget* WBP_Quest;

};
