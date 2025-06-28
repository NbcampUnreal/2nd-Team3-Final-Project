// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberSlot.h"

FEmberSlot::FEmberSlot(const FName& InItemID, const int32 InQuantity,
	const TArray<FItemEffectApplicationInfo>& InEnchantEffects) : FEmberMasterItemData(InItemID, InQuantity, InEnchantEffects)
{
	CashedEmberSlotData();
}

FEmberSlot::FEmberSlot(const FEmberItemEntry& EmberItemEntry)
{
	*this = FEmberSlot(EmberItemEntry.ItemID, EmberItemEntry.Quantity, EmberItemEntry.Enchants);

}

FEmberSlot::FEmberSlot(const FInstancedStruct& InInstancedStruct) : FEmberMasterItemData(InInstancedStruct)
{
	if (const FEmberSlot* InSlot = InInstancedStruct.GetPtr<FEmberSlot>())
	{
		SlotInfo = InSlot->SlotInfo;
	}
	else
	{
		CashedEmberSlotData();
	}
}

void FEmberSlot::CashedEmberSlotData()
{
	if (const FSlotInfoRow* SlotInfoRow = FindData<FSlotInfoRow>())
	{
		SlotInfo = *SlotInfoRow;
		MaxQuantity = SlotInfoRow->MaxStackSize;
	}
}
