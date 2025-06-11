// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberEquipmentManager.h"

#include "AbilitySystemComponent.h"
#include "AudioMixerDevice.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Core/ItemGamePlayTags.h"
#include "Core/ItemSystemLibrary.h"
#include "EmberLog/EmberLog.h"


UEmberEquipmentManager::UEmberEquipmentManager()
{
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Helmet);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Necklace);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Chest);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Legs);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Boots);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Weapon);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Cape);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Bracelet);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Gloves);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Belt);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Ring);
	SlotGameplayTags.AddTag(EmberEquipmentGameplayTags::Item_Equipment_Shield);
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
	FInstancedStruct ItemInfo = IEmberSlotDataProviderInterface::Execute_GetSlotItemInfo(AnotherProvider.GetObject(), IndexFrom);
	int32 AddItem = AddItemSlot(ItemInfo, IndexTo);
	IEmberSlotDataProviderInterface::Execute_RemoveItemFromSlot(AnotherProvider.GetObject(), IndexFrom, AddItem);
}

int32 UEmberEquipmentManager::RemoveItemFromSlot_Implementation(int32 SlotIndex, int32 QuantityToRemove)
{
	if (DataSlots.IsValidIndex(SlotIndex))
	{
		RemoveEffect(SlotIndex);
	}
	return Super::RemoveItemFromSlot_Implementation(SlotIndex, QuantityToRemove);
	
}

int32 UEmberEquipmentManager::AddDataInIndex(const FInstancedStruct& InItem, int32 InSlotIndex)
{
	int32 CurrentQuantity = 0;
	FInstancedStruct TmpInItem = InItem;
	
	if (DataSlots.IsValidIndex(InSlotIndex))
	{
		if (FEmberSlotData* InSlot = TmpInItem.GetMutablePtr<FEmberSlotData>())
		{
			FEquipmentSlotData NewSlotData = FEquipmentSlotData(*InSlot);
			if (!DataSlots[InSlotIndex].IsValid())
			{
				NewSlotData.Quantity = 0;
				NewSlotData.InitializeInstancedStruct(DataSlots[InSlotIndex]);
			}
			if (FEquipmentSlotData* Slot = DataSlots[InSlotIndex].GetMutablePtr<FEquipmentSlotData>())
			{
				if (Slot->ItemID.IsNone())
				{
					NewSlotData.InitializeInstancedStruct(DataSlots[InSlotIndex]);
					Slot = DataSlots[InSlotIndex].GetMutablePtr<FEquipmentSlotData>();
					Slot->Quantity = 0;
				}
				
				if (InSlot->ItemID == Slot->ItemID && NewSlotData.EquipmentInfo && 
					NewSlotData.EquipmentInfo->EquipmentTag.HasTag(SlotGameplayTags.GetByIndex(InSlotIndex)))
				{

					CurrentQuantity = FMath::Min(Slot->MaxStackSize - Slot->Quantity, InSlot->Quantity);
					CurrentQuantity = FMath::Max(CurrentQuantity, 0);

					Slot->Quantity += CurrentQuantity;

					
					FEmberItemKey ItemKey = FEmberItemKey(Slot->ItemID, Slot->EnchantEffects);
					FInstancedStruct& Data = TotalData.FindOrAdd(ItemKey);
					if (FEmberTotalSlot* ItemData = Data.GetMutablePtr<FEmberTotalSlot>())
					{
						ItemData->AddQuantity(CurrentQuantity);
						ItemData->AddIndex(InSlotIndex);
					}
					else
					{
						FEmberTotalSlot NewData = FEmberTotalSlot(InSlot->ItemID, InSlot->Quantity, InSlot->EnchantEffects);
						NewData.InitializeInstancedStruct(Data);
					}
                
					OnDataChangedDelegate.Broadcast(InSlotIndex, DataSlots[InSlotIndex]);
				}
			}
		}
	}

#if UE_BUILD_DEVELOPMENT
	EMBER_LOG(LogEmberItem, Display, TEXT("ReturnQuantity : %d"), CurrentQuantity);

#endif

    
	return CurrentQuantity;
}

int32 UEmberEquipmentManager::TryAddItems(const FInstancedStruct& InItem, int32 InSlotIndex)
{
	int AddedQuantity = 0;

	if (const FEmberSlotData* InSlot = InItem.GetPtr<FEmberSlotData>())
	{
		if (InSlot->ItemID.IsNone() || InSlot->Quantity <= 0)
		{
			return 0;
		}
		
		if (DataSlots.IsValidIndex(InSlotIndex))
		{
			RemoveEffect(InSlotIndex);
			DataSlots[InSlotIndex].Reset();
		}

		AddedQuantity = AddDataInIndex(InItem, InSlotIndex);

		ActiveEffect(InSlotIndex);
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

void UEmberEquipmentManager::RemoveEffect(int32 InSlotIndex)
{
	if (DataSlots.IsValidIndex(InSlotIndex))
	{
		if (FEmberSlotData* InSlot = DataSlots[InSlotIndex].GetMutablePtr<FEmberSlotData>())
		{
			FGameplayEffectQuery Query;
			if (OwnerAbilitySystemComponent)
			{
				for (FItemEffectApplicationInfo& Info : InSlot->EnchantEffects)
				{
					Query.EffectDefinition = Info.GameplayEffectClass;
						
					for (auto& Effect : OwnerAbilitySystemComponent->GetActiveEffects(Query))
					{
						if (Effect.IsValid())
						{
							if (const FActiveGameplayEffect* ActiveGameplayEffect = OwnerAbilitySystemComponent->GetActiveGameplayEffect(Effect))
							{
								float FoundValue = ActiveGameplayEffect->Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(Info.MagnitudeSetByCallerTag.GetTagName()));
								if (FMath::IsNearlyEqual(FoundValue, Info.Magnitude))
								{
									OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(Effect, 1);
								}
							}
						}
					}
				}
			}
		}
	}
}

void UEmberEquipmentManager::ActiveEffect(int32 InSlotIndex)
{
	if (DataSlots.IsValidIndex(InSlotIndex) && OwnerAbilitySystemComponent)
	{

		if (FEquipmentSlotData* InSlot = DataSlots[InSlotIndex].GetMutablePtr<FEquipmentSlotData>())
		{
			UItemSystemLibrary::ApplyEffectInfoList(OwnerAbilitySystemComponent, InSlot->MainEffect, Owner);
			UItemSystemLibrary::ApplyEffectInfoList(OwnerAbilitySystemComponent, InSlot->EnchantEffects, Owner);

		}
	}
}

bool UEmberEquipmentManager::bIsEquipmentTag(const FGameplayTag& InTag) const
{
	return SlotGameplayTags.HasTagExact(InTag);
}
