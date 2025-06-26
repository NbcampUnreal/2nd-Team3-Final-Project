// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/ItemStruct/EmberSlot.h"
#include "EmberInventorySlot.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FEmberInventorySlot : public FEmberSlot
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FConsumableInfoRow ConsumableInfo = FConsumableInfoRow();

	FEmberInventorySlot() = default;
	FEmberInventorySlot(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>());
	FEmberInventorySlot(const FInstancedStruct& InInstancedStruct);

	void CashedEmberInventoryData();

	virtual void Clear() override
	{
		Super::Clear();
		ConsumableInfo = FConsumableInfoRow();
	}
    
	virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
	{
		OutInstancedStruct.InitializeAs<FEmberInventorySlot>(*this);
	}
};
