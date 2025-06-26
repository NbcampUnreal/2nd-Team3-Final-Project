// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberInventorySlotContainer.h"

#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemStruct/Implements/EmberSlot/EmberInventorySlot.h"


int32 UEmberInventorySlotContainer::AddSlotItemReturnApplied(const FInstancedStruct& InInstancedStruct,
                                                             int32 InSlotIndex)
{
	int32 QuantityToAdd = Super::AddSlotItemReturnApplied(InInstancedStruct, InSlotIndex);
	if (const FEmberItemEntry* InItemEntry = InInstancedStruct.GetPtr<FEmberItemEntry>())
	{
		InSlotIndex = FindAddSlotIndex(*InItemEntry);
		FEmberItemEntry NewItemEntry = FEmberItemEntry(*InItemEntry);
		NewItemEntry.Quantity -= QuantityToAdd;
		while (InItemEntry->Quantity > QuantityToAdd && InSlotIndex < ItemSlots.Num())
		{
			QuantityToAdd += Super::AddSlotItemReturnApplied(InInstancedStruct, InSlotIndex);
		}
	}
	return QuantityToAdd;
}

int32 UEmberInventorySlotContainer::AddSlotItemReturnApplied(const FName& InItemID, const int32 InQuantity,
	int32 InSlotIndex, const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	return Super::AddSlotItemReturnApplied(InItemID, InQuantity, InSlotIndex, InEnchants);
}

int32 UEmberInventorySlotContainer::AddSlotItemReturnApplied(const FEmberItemEntry& InItemEntry, int32 InSlotIndex)
{
	return Super::AddSlotItemReturnApplied(InItemEntry, InSlotIndex);
}

int32 UEmberInventorySlotContainer::MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity)
{
	int32 MovedQuantity = Super::MoveItemByIndex(IndexTo, IndexForm, InQuantity);

	if (MovedQuantity <= 0)
	{
		SwapSlots(IndexTo, IndexForm);
	}
	return MovedQuantity;
}

int32 UEmberInventorySlotContainer::FindAddSlotIndex(const FEmberItemEntry& InItemEntry)
{
	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		FInstancedStruct& InstancedStruct = ItemSlots[Index];
		if (const FEmberItemEntry* ItemEntry = InstancedStruct.GetPtr<FEmberItemEntry>())
		{
			if (ItemEntry->CreateItemKey() == InItemEntry.CreateItemKey() && !ItemEntry->bIsFull())
			{
				return Index;
			}
		}
	}

	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		FInstancedStruct& InstancedStruct = ItemSlots[Index];
		if (const FEmberItemEntry* ItemEntry = InstancedStruct.GetPtr<FEmberItemEntry>())
		{
			if (ItemEntry->bIsEmpty())
			{
				return Index;
			}
		}
	}
	return ItemSlots.Num();
}

void UEmberInventorySlotContainer::CreateItemSlot(const FEmberItemEntry& InItemEntry, const int32 InItemIndex)
{
	if (ItemSlots.IsValidIndex(InItemIndex))
	{
		FEmberInventorySlot NewSlot = FEmberInventorySlot(InItemEntry.ItemID, InItemEntry.Quantity, InItemEntry.Enchants);
		NewSlot.InitializeInstancedStruct(ItemSlots[InItemIndex]);
	}
}


