// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/UI/SlotWidget/SlotsPanel/EmberBaseAutoCreateSlotsPanel.h"
#include "EmberInventoryPanel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberInventoryPanel : public UEmberBaseAutoCreateSlotsPanel
{
	GENERATED_BODY()

public:
	virtual void BP_SetProvider_Implementation() override;
	
};
