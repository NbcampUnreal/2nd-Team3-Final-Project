// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseDragAbleSlotWidget.h"
#include "EmberQuickSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberQuickSlotWidget : public UEmberBaseDragAbleSlotWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void UseQuickSlotItem(int32 InIndex) const;
};
