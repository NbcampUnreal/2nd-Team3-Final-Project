// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberInventorySlot.h"

FEmberInventorySlot::FEmberInventorySlot(const FName& InItemID, const int32 InQuantity,
	const TArray<FItemEffectApplicationInfo>& InEnchantEffects): FEmberSlot(InItemID, InQuantity, InEnchantEffects)
{
	CashedEmberInventoryData();
}

FEmberInventorySlot::FEmberInventorySlot(const FInstancedStruct& InInstancedStruct) : FEmberSlot(InInstancedStruct)
{
	if (const FEmberInventorySlot* InEmberInventorySlot = InInstancedStruct.GetPtr<FEmberInventorySlot>())
	{
		ConsumableInfo = InEmberInventorySlot->ConsumableInfo;
	}
	else 
	{
		CashedEmberInventoryData();
	}
}

void FEmberInventorySlot::CashedEmberInventoryData()
{
	if (const FConsumableInfoRow* InConsumableInfoRow = FindData<FConsumableInfoRow>())
	{
		ConsumableInfo = *InConsumableInfoRow;
	}
}
