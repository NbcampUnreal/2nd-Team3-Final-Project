// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberItemDeveloperSetting.generated.h"

class UEmberQuickSlotWidget;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class PROJECTEMBER_API UEmberItemDeveloperSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (DisplayName = "Item QuickSlot"))
	TSubclassOf<UUserWidget> QuickSlotWidgetClass;

	static TObjectPtr<const UEmberItemDeveloperSetting> Get()
	{
		return GetDefault<UEmberItemDeveloperSetting>();
	}
};
