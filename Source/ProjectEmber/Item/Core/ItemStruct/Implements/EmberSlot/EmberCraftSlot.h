// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/ItemCraftType.h"
#include "Item/Core/ItemStruct/EmberSlot.h"
#include "UObject/Object.h"
#include "EmberCraftSlot.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FEmberCraftSlot : public FEmberSlot
{
	GENERATED_BODY()

	TArray<FEmberItemEntry> RequiredItems;
	
	FEmberCraftSlot() = default;
	FEmberCraftSlot(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>()) : FEmberSlot(InItemID, InQuantity, InEnchantEffects) {};
	FEmberCraftSlot(const FInstancedStruct& InInstancedStruct) : FEmberSlot(InInstancedStruct) {};
	FEmberCraftSlot(const FCraftInfoRow& CraftTable);
	
	virtual void Clear() override
	{
		Super::Clear();
		RequiredItems.Reset();
	};

	virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
	{
		OutInstancedStruct.InitializeAs<FEmberCraftSlot>(*this);
	}
};