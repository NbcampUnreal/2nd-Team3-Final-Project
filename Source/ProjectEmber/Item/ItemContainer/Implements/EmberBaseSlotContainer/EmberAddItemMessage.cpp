// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberAddItemMessage.h"

#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemStruct/Implements/EmberMasterItemData/EmberDropMessageSlot.h"

UEmberAddItemMessage::UEmberAddItemMessage()
{
	SlotCount = 5;
}

void UEmberAddItemMessage::StartMessage()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UEmberAddItemMessage::UpdateItem, 1.0f, true);

}

int32 UEmberAddItemMessage::AddSlotItemReturnApplied(const FInstancedStruct& InInstancedStruct,
                                                             int32 InSlotIndex)
{
	int32 QuantityToAdd = 0;
	if (const FEmberItemEntry* InItemEntry = InInstancedStruct.GetPtr<FEmberItemEntry>())
	{
		InSlotIndex = FindAddSlotIndex(*InItemEntry);

		if (InSlotIndex >= ItemSlots.Num())
		{
			InSlotIndex = RemoveOldMessage();
		}
		QuantityToAdd += Super::AddSlotItemReturnApplied(InInstancedStruct, InSlotIndex);

		if (FEmberDropMessageSlot* Item = ItemSlots[InSlotIndex].GetMutablePtr<FEmberDropMessageSlot>())
		{
			Item->CurrentTime = GetWorld()->GetTimeSeconds();
		}
	}
	
	return QuantityToAdd;
}

int32 UEmberAddItemMessage::AddSlotItemReturnApplied(const FName& InItemID, const int32 InQuantity,
	int32 InSlotIndex, const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	return Super::AddSlotItemReturnApplied(InItemID, InQuantity, InSlotIndex, InEnchants);
}

int32 UEmberAddItemMessage::AddSlotItemReturnApplied(const FEmberItemEntry& InItemEntry, int32 InSlotIndex)
{
	return Super::AddSlotItemReturnApplied(InItemEntry, InSlotIndex);
}

int32 UEmberAddItemMessage::MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity)
{
	int32 MovedQuantity = Super::MoveItemByIndex(IndexTo, IndexForm, InQuantity);

	if (MovedQuantity <= 0)
	{
		SwapSlots(IndexTo, IndexForm);
	}
	return MovedQuantity;
}

int32 UEmberAddItemMessage::FindAddSlotIndex(const FEmberItemEntry& InItemEntry)
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

void UEmberAddItemMessage::CreateItemSlot(const FEmberItemEntry& InItemEntry, const int32 InItemIndex)
{
	if (ItemSlots.IsValidIndex(InItemIndex))
	{

		FEmberDropMessageSlot NewSlot = FEmberDropMessageSlot(InItemEntry.ItemID, InItemEntry.Quantity, InItemEntry.Enchants);
		NewSlot.InitializeInstancedStruct(ItemSlots[InItemIndex]);
		NewSlot.CurrentTime = GetWorld()->GetTimeSeconds();
	}
}

void UEmberAddItemMessage::UpdateItem()
{
	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		FInstancedStruct& Slot = ItemSlots[Index];

		if (FEmberDropMessageSlot* EmberDropMessageSlot = Slot.GetMutablePtr<FEmberDropMessageSlot>())
		{
			if (!EmberDropMessageSlot->ItemID.IsNone() && FMath::Abs(GetWorld()->GetTimeSeconds() - EmberDropMessageSlot->CurrentTime) > MessageLifeTime)
			{
				RemoveSlotItemReturnApplied(EmberDropMessageSlot->Quantity ,Index);
			}
		}
	}
}

int32 UEmberAddItemMessage::RemoveOldMessage()
{
	float MinTime = MessageLifeTime;
	int32 OldIndex= 0;
	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		FInstancedStruct Slot = ItemSlots[Index];
		if (FEmberDropMessageSlot* EmberDropMessageSlot = Slot.GetMutablePtr<FEmberDropMessageSlot>())
		{
			float CurrentTime = FMath::Abs(GetWorld()->GetTimeSeconds() - EmberDropMessageSlot->CurrentTime);
			if (CurrentTime < MinTime)
			{
				MinTime = CurrentTime;
				OldIndex = Index;
			}
		}
	}
	ItemSlots[OldIndex].Reset();
	return OldIndex;
}
