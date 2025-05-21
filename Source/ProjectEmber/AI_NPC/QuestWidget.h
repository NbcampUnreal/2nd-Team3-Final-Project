// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"


class UButton;
class UDataTable;

DECLARE_DELEGATE(FOnQuestAccepted)
DECLARE_DELEGATE(FOnQuestRefused)

UCLASS()
class PROJECTEMBER_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(meta = (BindWidget))
    UButton* AcceptButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RefuseButton;

    // 외부에서 설정해줄 퀘스트 데이터
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    UDataTable* QuestDataTable;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FName QuestRowName;

    // 델리게이트 바인딩
    FOnQuestAccepted OnQuestAccepted;
    FOnQuestRefused OnQuestRefused;

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void HandleAcceptClicked();

    UFUNCTION()
    void HandleRefuseClicked();
};
