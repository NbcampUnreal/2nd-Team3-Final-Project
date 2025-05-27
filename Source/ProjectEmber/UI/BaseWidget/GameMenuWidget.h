// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMenuWidget.generated.h"

/**
 * 
 */
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
};
