// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/ItemStruct/EmberSlot.h"
#include "EmberWidgetSlot.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FEmberWidgetSlot : public FEmberSlot
{
	GENERATED_BODY()

	FEmberWidgetSlot() = default;
	FEmberWidgetSlot(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>()) : FEmberSlot(InItemID, InQuantity, InEnchantEffects) {};
	FEmberWidgetSlot(const FEmberItemEntry& EmberItemEntry) : FEmberSlot(EmberItemEntry) {};
	FEmberWidgetSlot(const FInstancedStruct& InInstancedStruct) : FEmberSlot(InInstancedStruct) {};

	virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
	{
		OutInstancedStruct.InitializeAs<FEmberWidgetSlot>(*this);
	}
};