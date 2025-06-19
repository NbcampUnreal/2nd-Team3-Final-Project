// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/EmberTmpStruct.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"
#include "EmberCraftSelectSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberCraftSelectSlotWidget : public UEmberBaseSlotWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void InitCraftItems(const FEmberMasterItemData& InRequestItem);
	

};
