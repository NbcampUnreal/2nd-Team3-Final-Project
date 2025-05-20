// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseSlotsPanel.h"
#include "EmberEquipmentSlotsPanel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberEquipmentSlotsPanel : public UEmberBaseSlotsPanel
{
	GENERATED_BODY()
public:
	virtual void BP_CreateSlots_Implementation() override;
	virtual void BP_SetProvider_Implementation() override;

};
