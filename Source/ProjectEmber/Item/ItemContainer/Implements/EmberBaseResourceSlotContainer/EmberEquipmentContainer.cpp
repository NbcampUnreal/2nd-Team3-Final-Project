// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberEquipmentContainer.h"

#include "AbilitySystemComponent.h"
#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemGamePlayTags.h"
#include "Item/Core/ItemSystemLibrary.h"
#include "Item/Core/ItemStruct/Implements/EmberSlot/EmberEquipmentSlot.h"

UEmberEquipmentContainer::UEmberEquipmentContainer()
{
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Helmet);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Chest);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Legs);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Gloves);

	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Ring);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Tool);

	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Weapon);
	SlotCount = SlotGameplayTags.Num();

	EquipmentEffects.SetNum(SlotCount);
	ItemSlots.SetNum(SlotCount);
	for (FInstancedStruct& InstancedStruct : ItemSlots)
	{
		FEmberEquipmentSlot NewSlot = FEmberEquipmentSlot();
		NewSlot.InitializeInstancedStruct(InstancedStruct);
	}
}


bool UEmberEquipmentContainer::bAbleAddItemSlot(const FInstancedStruct& InItemEntry, int32 InSlotIndex)
{
	if (Super::bAbleAddItemSlot(InItemEntry, InSlotIndex))
	{
		if (const FEmberMasterItemData* EmberMasterItemData = InItemEntry.GetPtr<FEmberMasterItemData>())
		{
			if (const FEquipmentInfoRow* InfoRow = EmberMasterItemData->FindData<FEquipmentInfoRow>())
			{
				if (InfoRow->EquipmentTag.HasTag(SlotGameplayTags.GetByIndex(InSlotIndex)))
				{
					return true;
				}
			}
		}
		else
		{
			FEmberMasterItemData TmpEmberMasterItemData = FEmberMasterItemData(InItemEntry);
			if (const FEquipmentInfoRow* InfoRow = TmpEmberMasterItemData.FindData<FEquipmentInfoRow>())
			{
				if (InfoRow->EquipmentTag.HasTag(SlotGameplayTags.GetByIndex(InSlotIndex)))
				{
					return true;
				}
			}
		}
	}

	return false;
}

int32 UEmberEquipmentContainer::GetSlotIndex(const FGameplayTag& InTag) const
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

int32 UEmberEquipmentContainer::AddSlotItemReturnApplied(const FInstancedStruct& InInstancedStruct, int32 InSlotIndex)
{
	int32 QuantityToAdd = Super::AddSlotItemReturnApplied(InInstancedStruct, InSlotIndex);
	if (QuantityToAdd > 0)
	{
		ActiveEffect(InSlotIndex);
	}
	return QuantityToAdd;
}


int32 UEmberEquipmentContainer::RemoveSlotItemReturnApplied(const int32 InOutQuantity, int32 InSlotIndex)
{
	if (EquipmentEffects.IsValidIndex(InSlotIndex))
	{
		RemoveEffect(InSlotIndex);
	}
	return Super::RemoveSlotItemReturnApplied(InOutQuantity, InSlotIndex);
}

void UEmberEquipmentContainer::RemoveEffect(int32 InSlotIndex)
{
	if (EquipmentEffects.IsValidIndex(InSlotIndex))
	{
		for (FActiveGameplayEffectHandle& Effect : EquipmentEffects[InSlotIndex])
		{
			if (Effect.IsValid())
			{
				OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(Effect);
			}
		}
		EquipmentEffects[InSlotIndex].Reset();
	}
}

void UEmberEquipmentContainer::ActiveEffect(int32 InSlotIndex)
{
	if (EquipmentEffects.IsValidIndex(InSlotIndex) && OwnerAbilitySystemComponent)
	{
		if (FEmberEquipmentSlot* InSlot = ItemSlots[InSlotIndex].GetMutablePtr<FEmberEquipmentSlot>())
		{

			TArray<FActiveGameplayEffectHandle> EffectHandles;
			EffectHandles.Append(UItemSystemLibrary::ApplyEffectInfoList(OwnerAbilitySystemComponent, InSlot->MainEffectInfos, Owner));
			EffectHandles.Append(UItemSystemLibrary::ApplyEffectInfoList(OwnerAbilitySystemComponent, InSlot->Enchants, Owner));

			EquipmentEffects[InSlotIndex] = EffectHandles;
		}
	}
}

bool UEmberEquipmentContainer::bIsEquipmentTag(const FGameplayTag& InTag) const
{
	return SlotGameplayTags.HasTagExact(InTag);
}

void UEmberEquipmentContainer::CreateItemSlot(const FEmberItemEntry& InItemEntry, const int32 InItemIndex)
{
	if (ItemSlots.IsValidIndex(InItemIndex))
	{
		FEmberEquipmentSlot NewSlot = FEmberEquipmentSlot(InItemEntry.ItemID, InItemEntry.Quantity, InItemEntry.Enchants);
		NewSlot.InitializeInstancedStruct(ItemSlots[InItemIndex]);
	}
}
