// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberInventorySlot.h"
#include "Item/Core/ItemStruct/EmberSlot.h"
#include "UObject/Object.h"
#include "EmberQuickSlot.generated.h"

USTRUCT(BlueprintType)
struct FEmberQuickSlot : public FEmberInventorySlot
{
	GENERATED_BODY()

	FEmberQuickSlot() = default;
	FEmberQuickSlot(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>()) : FEmberInventorySlot(InItemID, InQuantity)
	{
		MaxQuantity = ITEM_SYSTEM_MAX_STACK;
	};
	FEmberQuickSlot(const FInstancedStruct& InInstancedStruct) : FEmberInventorySlot(InInstancedStruct)
	{
		MaxQuantity = ITEM_SYSTEM_MAX_STACK;
	};
};