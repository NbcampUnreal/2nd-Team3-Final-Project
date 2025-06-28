// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftContainer.h"

#include "EmberLog/EmberLog.h"

int32 UEmberCraftContainer::RemoveSlotItemReturnApplied(const int32 InOutQuantity, int32 InSlotIndex)
{
	int32 QuantityToRemove = 0;

	if (ItemSlots.IsValidIndex(InSlotIndex))
	{
		if (FEmberItemEntry* EmberItemEntry = ItemSlots[InSlotIndex].GetMutablePtr<FEmberItemEntry>())
		{
			QuantityToRemove = EmberItemEntry->RemoveQuantityReturnApplied(InOutQuantity);
			int32 TotalQuantityRemoved = RemoveItemReturnApplied(FEmberItemEntry(EmberItemEntry->ItemID, QuantityToRemove, EmberItemEntry->Enchants));

#if UE_BUILD_DEVELOPMENT
			EMBER_LOG(LogEmberItem, Display, TEXT("Remove[%d] : %s(%d / %d)"),InSlotIndex , *EmberItemEntry->ItemID.ToString(), QuantityToRemove, EmberItemEntry->Quantity);
#endif
			if (TotalQuantityRemoved != QuantityToRemove)
			{
				EMBER_LOG(LogEmberItem, Warning, TEXT("UnMatched Total : %d, Slot : %d"), TotalQuantityRemoved, QuantityToRemove);
			}
			OnItemChangedDelegate.Broadcast(InSlotIndex, ItemSlots[InSlotIndex]);
		}
	}
	return QuantityToRemove;
}
