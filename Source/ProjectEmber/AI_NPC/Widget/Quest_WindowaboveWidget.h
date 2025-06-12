// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest_WindowaboveWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UQuest_WindowaboveWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCloseButtonClicked();

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;
};