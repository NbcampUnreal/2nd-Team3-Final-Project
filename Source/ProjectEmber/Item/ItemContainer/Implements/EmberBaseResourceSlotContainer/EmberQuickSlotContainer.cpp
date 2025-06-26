// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberQuickSlotContainer.h"

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

	if (FEmberQuickSlot* QuickSlot = SlotInstance.GetMutablePtr<FEmberQuickSlot>())
	{
		if (!QuickSlot->bIsEmpty())
		{
			HandleItemConsumption(&QuickSlot->ConsumableInfo);
			UseQuantity = RemoveSlotItemReturnApplied(QuickSlot->ConsumableInfo.ConsumeAmount, InIndex);

		}
	}
        
#if UE_BUILD_DEVELOPMENT
	EMBER_LOG(LogEmberItem, Display, TEXT("UseAmount : %d"), UseQuantity);
#endif
   
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

void UEmberQuickSlotContainer::HandleItemConsumption(const FConsumableInfoRow* ConsumeData)
{
	if (!ConsumeData || !OwnerAbilitySystemComponent) return;

	UItemSystemLibrary::ApplyEffectInfoList(OwnerAbilitySystemComponent, ConsumeData->EffectsToApplyOnConsume, Owner); 

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

