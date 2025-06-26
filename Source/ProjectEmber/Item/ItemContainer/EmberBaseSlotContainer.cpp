// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseSlotContainer.h"

#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemStruct/EmberSlot.h"

UEmberBaseSlotContainer::UEmberBaseSlotContainer()
{
	ItemSlots.SetNum(SlotCount);
	for (FInstancedStruct& InstancedStruct : ItemSlots)
	{
		FEmberSlot NewSlot = FEmberSlot();
		NewSlot.InitializeInstancedStruct(InstancedStruct);
	}
}

void UEmberBaseSlotContainer::InitSlotCount(int32 InSlotCount, int32 InSlotMaxRow)
{
	SlotCount = InSlotCount;
	SlotMaxRow = InSlotMaxRow;
	ItemSlots.SetNum(SlotCount);
	for (FInstancedStruct& InstancedStruct : ItemSlots)
	{
		FEmberSlot NewSlot = FEmberSlot();
		NewSlot.InitializeInstancedStruct(InstancedStruct);
	}
}


void UEmberBaseSlotContainer::AddSlotItem(FEmberItemEntry& InOutItemEntry, int32 InSlotIndex)
{
	FInstancedStruct InstancedStruct = FInstancedStruct();;
	InOutItemEntry.InitializeInstancedStruct(InstancedStruct);
	AddSlotItem(InstancedStruct, InSlotIndex);
}

void UEmberBaseSlotContainer::AddSlotItem(const FName& InItemID, int32& InOutQuantity, int32 InSlotIndex,
	const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	FEmberItemEntry Entry = FEmberItemEntry(InItemID, InOutQuantity, InEnchants);
	AddSlotItem(Entry, InSlotIndex);
}

void UEmberBaseSlotContainer::AddSlotItem(FInstancedStruct& InOutInstancedStruct, int32 InSlotIndex)
{
	int32 QuantityToAdd = AddSlotItemReturnApplied(InOutInstancedStruct, InSlotIndex);

	if (FEmberItemEntry* EmberItemEntry = InOutInstancedStruct.GetMutablePtr<FEmberItemEntry>())
	{
		EmberItemEntry->Quantity -= QuantityToAdd;
		AddSlotItem(*EmberItemEntry, InSlotIndex);
	}
}

int32 UEmberBaseSlotContainer::AddSlotItemReturnApplied(const FEmberItemEntry& InItemEntry, int32 InSlotIndex)
{
	int32 QuantityToAdd = 0;
	FInstancedStruct InstancedStruct = FInstancedStruct();
	InItemEntry.InitializeInstancedStruct(InstancedStruct);
	QuantityToAdd = AddSlotItemReturnApplied(InstancedStruct, InSlotIndex);


	return QuantityToAdd;
}

int32 UEmberBaseSlotContainer::AddSlotItemReturnApplied(const FName& InItemID, const int32 InQuantity, int32 InSlotIndex,
	const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	FEmberItemEntry Entry = FEmberItemEntry(InItemID, InQuantity, InEnchants);
	return AddSlotItemReturnApplied(Entry, InSlotIndex);
}

int32 UEmberBaseSlotContainer::AddSlotItemReturnApplied(const FInstancedStruct& InInstancedStruct, int32 InSlotIndex)
{
	int32 QuantityToAdd = 0;
	if (bAbleAddItemSlot(InInstancedStruct, InSlotIndex))
	{
		if (const FEmberItemEntry* InEmberItemEntry = InInstancedStruct.GetPtr<FEmberItemEntry>())
		{
			FEmberItemEntry* EmberItemEntry = ItemSlots[InSlotIndex].GetMutablePtr<FEmberItemEntry>();
			if (!EmberItemEntry || EmberItemEntry->bIsEmpty())
			{
				CreateItemSlot(*InEmberItemEntry, InSlotIndex);
				EmberItemEntry = ItemSlots[InSlotIndex].GetMutablePtr<FEmberItemEntry>();
				QuantityToAdd = EmberItemEntry->Quantity;
			}
			else
			{
				QuantityToAdd = EmberItemEntry->AddQuantityReturnApplied(InEmberItemEntry->Quantity);
			}
			
			if (QuantityToAdd > 0)
			{
				OnItemChangedDelegate.Broadcast(InSlotIndex, ItemSlots[InSlotIndex]);

				FEmberItemEntry TempEmberItemEntry = FEmberItemEntry(*InEmberItemEntry);
				TempEmberItemEntry.Quantity = QuantityToAdd;
				int32 TotalQuantityAdded = AddItemReturnApplied(TempEmberItemEntry);

			
				if (TotalQuantityAdded != QuantityToAdd)
				{
					TempEmberItemEntry.Quantity = TotalQuantityAdded - QuantityToAdd;
					if (TempEmberItemEntry.Quantity > 0)
					{
						RemoveItemReturnApplied(TempEmberItemEntry);
					}
					else if (TempEmberItemEntry.Quantity < 0)
					{
						RemoveSlotItemReturnApplied(-TempEmberItemEntry.Quantity, InSlotIndex);
						QuantityToAdd = TotalQuantityAdded;
					}
				}
			}
				


#if UE_BUILD_DEVELOPMENT
			EMBER_LOG(LogEmberItem, Display, TEXT("Add[%d] : %s(%d / %d)"),InSlotIndex , *InEmberItemEntry->ItemID.ToString(), QuantityToAdd, EmberItemEntry->Quantity);

#endif
		}
	}

	return QuantityToAdd;
}

void UEmberBaseSlotContainer::RemoveSlotItem(int32& InOutQuantity, int32 InSlotIndex)
{
	int32 QuantityToRemove = RemoveSlotItemReturnApplied(InOutQuantity, InSlotIndex);
	InOutQuantity -= QuantityToRemove;
}

int32 UEmberBaseSlotContainer::RemoveSlotItemReturnApplied(const int32 InOutQuantity, int32 InSlotIndex)
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
			if (EmberItemEntry->bIsEmpty())
			{
				EmberItemEntry->Clear();
			}
			if (TotalQuantityRemoved != QuantityToRemove)
			{
				EMBER_LOG(LogEmberItem, Warning, TEXT("UnMatched Total : %d, Slot : %d"), TotalQuantityRemoved, QuantityToRemove);
			}
			OnItemChangedDelegate.Broadcast(InSlotIndex, ItemSlots[InSlotIndex]);
		}
	}
	return QuantityToRemove;
}

void UEmberBaseSlotContainer::SwapSlots(int32 InSlotIndex1, int32 InSlotIndex2)
{
	if (ItemSlots.IsValidIndex(InSlotIndex1) && ItemSlots.IsValidIndex(InSlotIndex2))
	{
		ItemSlots.Swap(InSlotIndex1, InSlotIndex2);
		OnItemChangedDelegate.Broadcast(InSlotIndex1, ItemSlots[InSlotIndex1]);
		OnItemChangedDelegate.Broadcast(InSlotIndex2, ItemSlots[InSlotIndex2]);
	}
}

int32 UEmberBaseSlotContainer::MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity)
{
	if (!ItemSlots.IsValidIndex(IndexTo) ||
	!ItemSlots.IsValidIndex(IndexForm))
	{
		EMBER_LOG(LogEmber, Warning, TEXT("CombineItemByIndex: Index is not valid. To : %d, Form : %d"), IndexTo, IndexForm);
		return 0;
	}
	int32 MovedQuantity = MergeSameItemSlot(IndexTo, IndexForm, InQuantity);

	return MovedQuantity;
}

FInstancedStruct UEmberBaseSlotContainer::GetInstancedItemSlotInfo(const int32 ItemIndex) const
{
	if (ItemSlots.IsValidIndex(ItemIndex))
	{
		return ItemSlots[ItemIndex];
	}
	return FInstancedStruct();
}

void UEmberBaseSlotContainer::CreateItemSlot(const FEmberItemEntry& InItemEntry, const int32 InItemIndex)
{
	if (ItemSlots.IsValidIndex(InItemIndex))
	{
		FEmberSlot NewSlot = FEmberSlot(InItemEntry.ItemID, InItemEntry.Quantity, InItemEntry.Enchants);
		NewSlot.InitializeInstancedStruct(ItemSlots[InItemIndex]);
	}
}

void UEmberBaseSlotContainer::DeleteItemSlot(const int32 InItemIndex)
{
	if (ItemSlots.IsValidIndex(InItemIndex))
	{
		if (FEmberItemEntry* ItemEntry = ItemSlots[InItemIndex].GetMutablePtr<FEmberItemEntry>())
		{
			ItemEntry->Clear();
		}
	}
}

bool UEmberBaseSlotContainer::bAbleAddItemSlot(const FInstancedStruct& InItemEntry, int32 InSlotIndex)
{
	if (ItemSlots.IsValidIndex(InSlotIndex))
	{
		if (const FEmberItemEntry* EmberItemEntry = ItemSlots[InSlotIndex].GetPtr<FEmberItemEntry>())
		{
			if (const FEmberItemEntry* InEmberItemEntry = InItemEntry.GetPtr<FEmberItemEntry>())
			{
				if (EmberItemEntry->ItemID.IsNone() || (EmberItemEntry->ItemID == InEmberItemEntry->ItemID && EmberItemEntry->bAbleAddQuantity(InEmberItemEntry->Quantity)))
				{

					return true;
				}
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

int32 UEmberBaseSlotContainer::MergeSameItemSlot(int32 SlotIndexTo, int32 SlotIndexFrom, int32 MergeQuantity)
{
	if (!ItemSlots.IsValidIndex(SlotIndexTo) || !ItemSlots.IsValidIndex(SlotIndexFrom))
	{
		EMBER_LOG(LogEmberItem, Error, TEXT("UEmberDataContainer::MergeItemSlot: Invalid Index"));
		return 0;
	}

    
	FEmberSlot* SlotFrom = ItemSlots[SlotIndexFrom].GetMutablePtr<FEmberSlot>();
	FEmberSlot* SlotTo = ItemSlots[SlotIndexTo].GetMutablePtr<FEmberSlot>();
	int MovedAmount = 0;

	if (SlotFrom && SlotTo)
	{
		if (SlotFrom->ItemID == SlotTo->ItemID)
		{
			int CurrentMaxQuantity = FMath::Min(SlotTo->MaxQuantity - SlotTo->Quantity, MergeQuantity);
			MovedAmount = FMath::Min(SlotFrom->Quantity, CurrentMaxQuantity);
			MovedAmount = FMath::Max(MovedAmount, 0);
			
			SlotFrom->Quantity -= MovedAmount;
			SlotTo->Quantity += MovedAmount;
			if (SlotFrom->Quantity <= 0)
			{
				SlotFrom->Clear();
			}
			
#if UE_BUILD_DEVELOPMENT
			EMBER_LOG(LogEmberItem, Display, TEXT("Merge : %d(%d[%d] -> %d[%d])"), MovedAmount, SlotFrom->Quantity, SlotIndexFrom, SlotTo->Quantity, SlotIndexTo);

#endif
			OnItemChangedDelegate.Broadcast(SlotIndexFrom, ItemSlots[SlotIndexFrom]);
			OnItemChangedDelegate.Broadcast(SlotIndexTo, ItemSlots[SlotIndexTo]);

			return MovedAmount;
		}
	}
	return MovedAmount;
}


int32 UEmberBaseSlotContainer::GetSlotCount_Implementation() const
{
	return SlotCount;
}

FInstancedStruct UEmberBaseSlotContainer::GetSlotItemInfo_Implementation(int32 InIndex) const
{
	return GetInstancedItemSlotInfo(InIndex);
}

void UEmberBaseSlotContainer::AddItemByWidget_Implementation(FEmberItemEntry& InOutItem, int32 InSlotIndex)
{
	AddSlotItem(InOutItem, InSlotIndex);
}

int32 UEmberBaseSlotContainer::RemoveItemFromSlot_Implementation(int32 InSlotIndex, int32 QuantityToRemove)
{
	return RemoveSlotItemReturnApplied(QuantityToRemove, InSlotIndex);
}

void UEmberBaseSlotContainer::MoveItemByWidget_Implementation(const FGameplayTag& SlotTag, int32 IndexTo,
	const TScriptInterface<UEmberSlotProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity)
{
	if (AnotherProvider.GetObject() != this)
	{
		FInstancedStruct InInstancedStruct = IEmberSlotProviderInterface::Execute_GetSlotItemInfo(AnotherProvider.GetObject(), IndexFrom);

		int32 QuantityToAdd = AddSlotItemReturnApplied(InInstancedStruct, IndexTo);
		IEmberSlotProviderInterface::Execute_RemoveItemFromSlot(AnotherProvider.GetObject(), IndexFrom, QuantityToAdd);
	}
	else
	{
		MoveItemByIndex(IndexTo, IndexFrom, Quantity);
	}
}

int32 UEmberBaseSlotContainer::GetSlotMaxRow_Implementation() const
{
	return SlotMaxRow;
}

void UEmberBaseSlotContainer::UseItemInSlot_Implementation(int32 SlotIndex)
{
}
