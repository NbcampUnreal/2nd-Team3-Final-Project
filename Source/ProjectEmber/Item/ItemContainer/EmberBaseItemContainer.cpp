// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseItemContainer.h"

#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemStruct/EmberItemEntry.h"

void UEmberBaseItemContainer::AddItem(const FName& InItemID, int32& InOutQuantity,
                                      const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	FEmberItemEntry InOutEntry = FEmberItemEntry(InItemID, InOutQuantity, InEnchants);
	AddItem(InOutEntry);
	InOutQuantity = InOutEntry.Quantity;
}

void UEmberBaseItemContainer::AddItem(FEmberItemEntry& InOutItemEntry)
{
	FInstancedStruct InstancedStruct;
	InOutItemEntry.InitializeInstancedStruct(InstancedStruct);
	AddItem(InstancedStruct);
}

void UEmberBaseItemContainer::AddItem(FInstancedStruct& InOutInstancedStruct)
{
	if (FEmberItemEntry* EmberItemEntry = InOutInstancedStruct.GetMutablePtr<FEmberItemEntry>())
	{
		int32 QuantityToAdd = AddItemReturnApplied(InOutInstancedStruct);
		EmberItemEntry->Quantity -= QuantityToAdd;

	}

	
}

int32 UEmberBaseItemContainer::AddItemReturnApplied(const FName& InItemID, const int32 InQuantity,
	const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	FEmberItemEntry InEntry = FEmberItemEntry(InItemID, InQuantity, InEnchants);
	int32 AddedQuantity = AddItemReturnApplied(InEntry);
	return AddedQuantity;
}

int32 UEmberBaseItemContainer::AddItemReturnApplied(const FEmberItemEntry& InItemEntry)
{
	FInstancedStruct InstancedStruct = FInstancedStruct();
	InItemEntry.InitializeInstancedStruct(InstancedStruct);
	int32 AddedQuantity = AddItemReturnApplied(InstancedStruct);
	return AddedQuantity;

}

int32 UEmberBaseItemContainer::AddItemReturnApplied(const FInstancedStruct& InInstancedStruct)
{
	int32 QuantityToAdd = 0;

	if (const FEmberItemEntry* InEmberItemEntry = InInstancedStruct.GetPtr<FEmberItemEntry>())
	{

		if (FEmberItemEntry* EmberItemEntry = GetItemInfo<FEmberItemEntry>(*InEmberItemEntry))
		{
			QuantityToAdd = EmberItemEntry->AddQuantityReturnApplied(InEmberItemEntry->Quantity);

		}
		else
		{
			FEmberItemEntry ItemEntry = *InEmberItemEntry;
			// 각 컨테이너에 들어갈 데이터를 선언할것
			CreateItem(ItemEntry);
			QuantityToAdd = ItemEntry.Quantity;
		}
		
#if UE_BUILD_DEVELOPMENT
		if (FEmberItemEntry* EmberItemEntry = GetItemInfo<FEmberItemEntry>(*InEmberItemEntry))
		{
			EMBER_LOG(LogEmberItem, Display, TEXT("Add : %s(%d / %d)"),*InEmberItemEntry->ItemID.ToString(), QuantityToAdd, EmberItemEntry->Quantity);
		}
		else
		{
			EMBER_LOG(LogEmberItem, Warning, TEXT("EmberItemEntry NotFind"));
		}
#endif
	}

	return QuantityToAdd;
}

void UEmberBaseItemContainer::RemoveItem(const FName& InItemID, int32& InOutQuantity,
	const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	FEmberItemEntry InOutEntry = FEmberItemEntry(InItemID, InOutQuantity, InEnchants);
	RemoveItem(InOutEntry);
	InOutQuantity = InOutEntry.Quantity;
}

void UEmberBaseItemContainer::RemoveItem(FEmberItemEntry& InOutItemEntry)
{
	FInstancedStruct InstancedStruct = FInstancedStruct();
	InOutItemEntry.InitializeInstancedStruct(InstancedStruct);
	RemoveItem(InstancedStruct);
}

void UEmberBaseItemContainer::RemoveItem(FInstancedStruct& InOutInstancedStruct)
{
	if (FEmberItemEntry* EmberItemEntry = InOutInstancedStruct.GetMutablePtr<FEmberItemEntry>())
	{
		int32 QuantityToAdd = RemoveItemReturnApplied(InOutInstancedStruct);
		EmberItemEntry->Quantity -= QuantityToAdd;
	}
}

int32 UEmberBaseItemContainer::RemoveItemReturnApplied(const FName& InItemID, const int32 InQuantity,
	const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	FEmberItemEntry InEntry = FEmberItemEntry(InItemID, InQuantity, InEnchants);
	int32 RemovedQuantity = RemoveItemReturnApplied(InEntry);
	return RemovedQuantity;
}

int32 UEmberBaseItemContainer::RemoveItemReturnApplied(const FEmberItemEntry& InItemEntry)
{
	int32 RemovedQuantity = 0;
	FInstancedStruct InstancedStruct = FInstancedStruct();
	InItemEntry.InitializeInstancedStruct(InstancedStruct);
	RemovedQuantity = RemoveItemReturnApplied(InstancedStruct);
	
	return RemovedQuantity;
}

int32 UEmberBaseItemContainer::RemoveItemReturnApplied(const FInstancedStruct& InInstancedStruct)
{
	int32 RemovedQuantity = 0;
	if (const FEmberItemEntry* InEmberItemEntry = InInstancedStruct.GetPtr<FEmberItemEntry>())
	{
		if (FEmberItemEntry* EmberItemEntry = GetItemInfo<FEmberItemEntry>(InInstancedStruct))
		{
			RemovedQuantity = EmberItemEntry->RemoveQuantityReturnApplied(InEmberItemEntry->Quantity);

#if UE_BUILD_DEVELOPMENT
			EMBER_LOG(LogEmberItem, Display, TEXT("Remove : %s(%d / %d)"),*InEmberItemEntry->ItemID.ToString(), RemovedQuantity, EmberItemEntry->Quantity);
#endif
		
			if (EmberItemEntry->bIsEmpty())
			{
				EmberItemEntry->Clear();
				DeleteItem(*EmberItemEntry);
			}
		
		}
	}
	return RemovedQuantity;
}

void UEmberBaseItemContainer::InitOwner(AActor* InOwner)
{
	Owner = InOwner;
}


FInstancedStruct* UEmberBaseItemContainer::GetInstancedItemInfo(const FName& InItemID,
                                                               const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	return GetInstancedItemInfo(FEmberItemKey(InItemID, InEnchants));
}

FInstancedStruct* UEmberBaseItemContainer::GetInstancedItemInfo(const FEmberItemKey& InBaseItem)
{
	return Items.Find(InBaseItem);
}

void UEmberBaseItemContainer::CreateItem(const FEmberItemEntry& InItemEntry)
{
	FInstancedStruct InstancedStruct = FInstancedStruct();
	FEmberItemEntry NewItemEntry = InItemEntry;
	NewItemEntry.InitializeInstancedStruct(InstancedStruct);
	Items.Add(NewItemEntry.CreateItemKey(), InstancedStruct);
}



void UEmberBaseItemContainer::DeleteItem(const FEmberItemEntry& InItemEntry)
{
	Items.Remove(InItemEntry.CreateItemKey());
}

