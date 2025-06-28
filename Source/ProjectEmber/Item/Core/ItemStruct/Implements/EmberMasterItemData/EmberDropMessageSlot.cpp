// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropMessageSlot.h"

FEmberDropMessageSlot::FEmberDropMessageSlot(const FName& InItemID, const int32 InQuantity,
                                             const TArray<FItemEffectApplicationInfo>& InEnchantEffects) : FEmberMasterItemData(InItemID, InQuantity, InEnchantEffects)
{
	CashedDropMessageSlotData();
}

FEmberDropMessageSlot::FEmberDropMessageSlot(const FEmberItemEntry& EmberItemEntry)
{
	*this = FEmberDropMessageSlot(EmberItemEntry.ItemID, EmberItemEntry.Quantity, EmberItemEntry.Enchants);

}

FEmberDropMessageSlot::FEmberDropMessageSlot(const FInstancedStruct& InInstancedStruct) : FEmberMasterItemData(InInstancedStruct)
{
	if (const FEmberDropMessageSlot* InSlot = InInstancedStruct.GetPtr<FEmberDropMessageSlot>())
	{
		SlotInfo = InSlot->SlotInfo;
	}
	else
	{
		CashedDropMessageSlotData();
	}
}

void FEmberDropMessageSlot::CashedDropMessageSlotData()
{
	if (const FSlotInfoRow* SlotInfoRow = FindData<FSlotInfoRow>())
	{
		SlotInfo = *SlotInfoRow;
	}
}
