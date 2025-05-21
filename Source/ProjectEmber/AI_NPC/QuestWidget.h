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

    // �ܺο��� �������� ����Ʈ ������
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    UDataTable* QuestDataTable;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FName QuestRowName;

    // ��������Ʈ ���ε�
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
