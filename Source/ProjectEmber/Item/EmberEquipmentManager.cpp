// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberEquipmentManager.h"

#include "AudioMixerDevice.h"
#include "Core/ItemGamePlayTags.h"
#include "EmberLog/EmberLog.h"


UEmberEquipmentManager::UEmberEquipmentManager()
{
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Head);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Body);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Arm);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Leg);
}

int32 UEmberEquipmentManager::GetSlotCount_Implementation() const
{
	return SlotGameplayTags.Num();
}

int32 UEmberEquipmentManager::GetSlotIndex(const FGameplayTag& InTag) const
{
	for (int32 Index = 0; Index < SlotGameplayTags.Num(); Index++)
	{
		if (SlotGameplayTags.GetByIndex(Index).MatchesTagExact(InTag))
		{
			return Index;
		}
	}
	return -1;
}

void UEmberEquipmentManager::MoveItemByWidget_Implementation(const FGameplayTag& InSlotTag, int32 IndexTo,
	const TScriptInterface<UEmberSlotDataProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity)
{
	FName ItemID = IEmberSlotDataProviderInterface::Execute_GetSlotItemID(AnotherProvider.GetObject(), IndexFrom);
	int32 AddedQuantity = 0;
	if (ItemID.IsValid())
	{
		FInstancedStruct SlotInstance = GetSlotDataByIndex(IndexTo);
		if (FEquipmentSlotData* Slot = SlotInstance.GetMutablePtr<FEquipmentSlotData>())
		{
			FName HasItemID = Slot->ItemID;
			FItemPair Item = FItemPair(ItemID, Quantity);
			AddedQuantity = TryAddItemsToSlots(Item, IndexTo);

			IEmberSlotDataProviderInterface::Execute_RemoveItemFromSlot(AnotherProvider.GetObject(), IndexFrom, AddedQuantity);
			
			if (HasItemID.IsValid())
			{
				IEmberSlotDataProviderInterface::Execute_AddItem(AnotherProvider.GetObject(), FItemPair(HasItemID, Slot->Quantity), IndexFrom);
			}
			
		}
	}
	
#if UE_BUILD_DEVELOPMENT
	EMBER_LOG(LogEmberItem, Display, TEXT("ReturnNum : %d"), AddedQuantity);
#endif
    
}

int32 UEmberEquipmentManager::TryAddItemsToSlots(const FItemPair& InItem, int32 InSlotIndex)
{
	
	if (InItem.ItemID.IsNone() || InItem.Quantity <= 0)
	{
		return 0;
	}

	int AddedQuantity = 0;
 
	if (DataSlots.IsValidIndex(InSlotIndex))
	{
		AddedQuantity = AddDataInIndex(InItem, InSlotIndex);
	}

	
#if UE_BUILD_DEVELOPMENT
	EMBER_LOG(LogEmberItem, Display, TEXT("ReturnNum : %d"), AddedQuantity);
#endif
    
	return AddedQuantity;
}

void UEmberEquipmentManager::InitializeInventorySlots()
{
	SlotCapacity = SlotGameplayTags.Num();
	DataSlots.SetNum(SlotCapacity);

	for (FInstancedStruct& Data : DataSlots)
	{
		FEquipmentSlotData NewSlot;
		NewSlot.InitializeInstancedStruct(Data);
	}
}

int32 UEmberEquipmentManager::AddDataInIndex(const FItemPair& InItem, int32 InSlotIndex)
{
	int32 AddedQuantity = 0;
	
	if (DataSlots.IsValidIndex(InSlotIndex))
	{
		if (FEquipmentSlotData* Slot = DataSlots[InSlotIndex].GetMutablePtr<FEquipmentSlotData>())
		{
			FEquipmentSlotData NewSlot = FEquipmentSlotData(InItem.ItemID);

			if (NewSlot.EquipmentInfo.IsSet() && NewSlot.EquipmentInfo->EquipmentTag.MatchesTagExact(SlotGameplayTags.GetByIndex(InSlotIndex)))
			{
				AddedQuantity =  AddItemSlotIndex(NewSlot, InItem.Quantity);
				*Slot = NewSlot;
				if (AddedQuantity > 0)
				{
					OnDataChangedDelegate.Broadcast(InSlotIndex, DataSlots[InSlotIndex]);
				}

				return AddedQuantity;
			}
		}
	}
	
#if UE_BUILD_DEVELOPMENT
	EMBER_LOG(LogEmberItem, Display, TEXT("ReturnNum : %d"), AddedQuantity);
#endif
    
	return AddedQuantity;
}

bool UEmberEquipmentManager::bIsEquipmentTag(const FGameplayTag& InTag) const
{
	return SlotGameplayTags.HasTagExact(InTag);
}
