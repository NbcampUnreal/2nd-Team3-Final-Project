// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/ItemStruct/EmberItemEntry.h"
#include "UObject/Object.h"
#include "EmberSaveEntry.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FEmberSaveEntry : public FEmberItemEntry
{
	GENERATED_BODY()
 
	int32 SlotIndex = 0;
	
	FEmberSaveEntry() = default;
    
	FEmberSaveEntry(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>()) : FEmberItemEntry(InItemID, InQuantity, InEnchantEffects) {};
	FEmberSaveEntry(const FEmberItemEntry& EmberItemEntry) : FEmberItemEntry(EmberItemEntry) {};
	
	FEmberSaveEntry(const FInstancedStruct& InInstancedStruct) : FEmberItemEntry(InInstancedStruct) {};
	
	virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
	{
		OutInstancedStruct.InitializeAs<FEmberSaveEntry>(*this);
	}

	virtual void Clear() override
	{
		Super::Clear();
		SlotIndex = 0;
	}
};
