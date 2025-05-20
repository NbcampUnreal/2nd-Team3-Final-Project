// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberEquipmentSlotsPanel.h"

#include "GameplayTagsManager.h"
#include "Components/GridPanel.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "Item/UserItemManger.h"
#include "Item/EmberEquipmentManager.h"
#include "Item/Core/ItemGamePlayTags.h"
#include "Item/UI/SlotWidget/Slot/EmberEquipmentSlotWidget.h"

void UEmberEquipmentSlotsPanel::BP_CreateSlots_Implementation()
{
	int32 MaxSlots = IEmberSlotDataProviderInterface::Execute_GetSlotCount(DataProvider.GetObject());

	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	Slots.SetNum(MaxSlots);
	TObjectPtr<UEmberEquipmentManager> EquipmentManager = Cast<UEmberEquipmentManager>(DataProvider.GetObject());
	if (!EquipmentManager)
	{
		EMBER_LOG(LogTemp, Error, TEXT("DataProvider is Null!"));

		return;
	}

	for (TObjectPtr<UWidget> Widget : SlotsPanel->GetAllChildren())
	{
		if (TObjectPtr<UEmberEquipmentSlotWidget> EquipmentSlot = Cast<UEmberEquipmentSlotWidget>(Widget))
		{
			if (EquipmentManager->bIsEquipmentTag(EquipmentSlot->EquipmentTypeTag))
			{
				int32 Index = EquipmentManager->GetSlotIndex(EquipmentSlot->EquipmentTypeTag);
				if (Index < 0)
				{
					continue;
				}
				EquipmentSlot->InitSlot(Index, DataProvider);
				Slots[Index] = EquipmentSlot;
			}
		}
	}
}

void UEmberEquipmentSlotsPanel::BP_SetProvider_Implementation()
{
	Super::BP_SetProvider_Implementation();
	
	if (GetOwningPlayer() && GetOwningPlayer()->GetCharacter())
	{
		if (TObjectPtr<UUserItemManger> ItemManger = GetOwningPlayer()->GetCharacter()->GetComponentByClass<UUserItemManger>())
		{
			DataProvider = ItemManger->GetEquipmentManager();
		}
	}
}
