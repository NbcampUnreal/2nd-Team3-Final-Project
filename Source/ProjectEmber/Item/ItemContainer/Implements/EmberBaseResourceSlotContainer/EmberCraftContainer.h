// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/ItemStruct/EmberSlot.h"
#include "Item/ItemContainer/EmberBaseResourceSlotContainer.h"
#include "EmberCraftContainer.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTEMBER_API UEmberCraftContainer : public UEmberBaseResourceSlotContainer
{
	GENERATED_BODY()
public:

	virtual int32 RemoveSlotItemReturnApplied(const int32 InOutQuantity, int32 InSlotIndex = -1) override;
};
