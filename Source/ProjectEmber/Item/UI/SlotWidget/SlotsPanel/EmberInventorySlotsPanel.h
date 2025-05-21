// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseSlotsPanel.h"
#include "EmberInventorySlotsPanel.generated.h"

struct FInventorySlotData;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberInventorySlotsPanel : public UEmberBaseSlotsPanel
{
	GENERATED_BODY()

public:
	virtual void BP_SetProvider_Implementation() override;
};
