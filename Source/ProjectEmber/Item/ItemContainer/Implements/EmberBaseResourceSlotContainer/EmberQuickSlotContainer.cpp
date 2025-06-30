// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberQuickSlotContainer.h"

#include "EmberInventorySlotContainer.h"
#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemSystemLibrary.h"
#include "Item/Core/ItemStruct/Implements/EmberSlot/EmberQuickSlot.h"

void UEmberQuickSlotContainer::UseSlotItem(int32 InIndex)
{
	if (!ItemSlots.IsValidIndex(InIndex))
	{
		EMBER_LOG(LogEmberItem, Warning, TEXT("UseItemInSlot: non Invalid slot index %d"), InIndex);
		return;
	}

	int32 UseQuantity = 0;
	FInstancedStruct& SlotInstance = ItemSlots[InIndex];

	if (FEmberInventorySlot* QuickSlot = SlotInstance.GetMutablePtr<FEmberInventorySlot>())
	{
		if (!QuickSlot->bIsEmpty())
		{

			HandleItemConsumption(&QuickSlot->ConsumableInfo);
			
			if (InventorySlotContainer)
			{
				TArray<FEmberItemEntry> RemoveItems;
				FEmberItemEntry RemoveItem = FEmberItemEntry(QuickSlot->ItemID, QuickSlot->ConsumableInfo.ConsumeAmount, QuickSlot->Enchants);
				RemoveItems.Add(RemoveItem);
				IEmberResourceProvider::Execute_RemoveResourceUntilAble(InventorySlotContainer, RemoveItems);
			}
#if UE_BUILD_DEVELOPMENT
			EMBER_LOG(LogEmberItem, Display, TEXT("UseAmount :%s[%d]: %d"), *QuickSlot->ItemID.ToString(), InIndex,UseQuantity);
#endif
		}
	}
        
#if UE_BUILD_DEVELOPMENT
	EMBER_LOG(LogEmberItem, Display, TEXT("UseAmount : %d, %d"), UseQuantity, InIndex);
#endif
  
}

void UEmberQuickSlotContainer::ConnectInventorySlotContainer(UEmberInventorySlotContainer* InInventorySlotContainer)
{
	InventorySlotContainer = InInventorySlotContainer;
	if (InventorySlotContainer)
	{
		InventorySlotContainer->OnItemChangedDelegate.AddDynamic(this, &UEmberQuickSlotContainer::InventoryChanged);
	}
}

FName UEmberQuickSlotContainer::SelectQuickSlot(int32 InIndex)
{
	if (CurrentQuickSlotIndex == InIndex)
	{
		CurrentQuickSlotIndex = -1;
		return FName();
	}
	
	if (ItemSlots.IsValidIndex(InIndex))
	{
		CurrentQuickSlotIndex = InIndex;
		if (const FEmberQuickSlot* EmberQuickSlot = ItemSlots[InIndex].GetPtr<FEmberQuickSlot>())
		{
			return EmberQuickSlot->ItemID;
		}
	}
	return FName();
}

void UEmberQuickSlotContainer::UseItemInSlot_Implementation(int32 SlotIndex)
{
	UseSlotItem(SlotIndex);
}

void UEmberQuickSlotContainer::MoveItemByWidget_Implementation(const FGameplayTag& SlotTag, int32 IndexTo,
	const TScriptInterface<UEmberSlotProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity)
{
	if (AnotherProvider.GetObject() != this)
	{
		FInstancedStruct InInstancedStruct = IEmberSlotProviderInterface::Execute_GetSlotItemInfo(AnotherProvider.GetObject(), IndexFrom);
		FEmberItemEntry InEmberItemEntry = FEmberItemEntry(InInstancedStruct);
		FInstancedStruct OutInstancedStruct;
		IEmberResourceProvider::Execute_GetItemInfo(AnotherProvider.GetObject(), InEmberItemEntry, OutInstancedStruct);

		AddSlotItemReturnApplied(OutInstancedStruct, IndexTo);
	}
	else
	{
		MoveItemByIndex(IndexTo, IndexFrom, Quantity);
	}
}

int32 UEmberQuickSlotContainer::AddSlotItemReturnApplied(const FInstancedStruct& InInstancedStruct, int32 InSlotIndex)
{
	if (ItemSlots.IsValidIndex(InSlotIndex))
	{
		if (const FEmberItemEntry* InItemEntry = InInstancedStruct.GetPtr<FEmberItemEntry>())
		{
			if (!InItemEntry->bIsEmpty())
			{
				CreateItemSlot(FEmberItemEntry(), InSlotIndex);
			}
		}
	}
	return Super::AddSlotItemReturnApplied(InInstancedStruct, InSlotIndex);
}

void UEmberQuickSlotContainer::InventoryChanged(int32 InIndex, const FInstancedStruct& InSlotData)
{
	if (const FEmberItemEntry* InItemEntry = InSlotData.GetPtr<FEmberItemEntry>())
	{
		if (!InItemEntry->bIsEmpty())
		{
			for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
			{
				FInstancedStruct& InInstancedStruct = ItemSlots[Index];
				if (FEmberItemEntry* ItemEntry = InInstancedStruct.GetMutablePtr<FEmberItemEntry>())
				{
					if (ItemEntry->ItemID == InItemEntry->ItemID)
					{
						ItemEntry->Quantity = InItemEntry->Quantity;
						OnItemChangedDelegate.Broadcast(Index, ItemSlots[Index]);
					}
				}
			}
		}
		else
		{
			UpdateRemoveItem();

		}
	}
	
}

void UEmberQuickSlotContainer::UpdateRemoveItem()
{
	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		FInstancedStruct& InstancedStruct = ItemSlots[Index];
		if (FEmberItemEntry* InItemEntry = InstancedStruct.GetMutablePtr<FEmberItemEntry>())
		{
			
			FInstancedStruct OutInstancedStruct = FInstancedStruct();
			IEmberResourceProvider::Execute_GetItemInfo(InventorySlotContainer, *InItemEntry, OutInstancedStruct);
			if (const FEmberItemEntry* ItemEntry = OutInstancedStruct.GetPtr<FEmberItemEntry>())
			{
				if (ItemEntry->bIsEmpty())
				{
					CreateItemSlot(FEmberItemEntry(), Index);
					OnItemChangedDelegate.Broadcast(Index, ItemSlots[Index]);
				}
			}
		}
	}
}

void UEmberQuickSlotContainer::CreateItemSlot(const FEmberItemEntry& InItemEntry, const int32 InItemIndex)
{
	if (ItemSlots.IsValidIndex(InItemIndex))
	{
		FEmberQuickSlot NewSlot = FEmberQuickSlot(InItemEntry.ItemID, InItemEntry.Quantity, InItemEntry.Enchants);
		NewSlot.InitializeInstancedStruct(ItemSlots[InItemIndex]);
		
	}
}

int32 UEmberQuickSlotContainer::MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity)
{
	int32 MovedQuantity = Super::MoveItemByIndex(IndexTo, IndexForm, InQuantity);
	if (MovedQuantity <= 0)
	{
		SwapSlots(IndexTo, IndexForm);
	}
	return MovedQuantity;
}

void UEmberQuickSlotContainer::HandleItemConsumption(const FConsumableInfoRow* ConsumeData)
{
	if (!ConsumeData || !OwnerAbilitySystemComponent) return;

	UItemSystemLibrary::ApplyEffectInfoList(OwnerAbilitySystemComponent, ConsumeData->EffectsToApplyOnConsume, Owner); 

}

