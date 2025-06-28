// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberInventoryPanel.h"

#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "Item/UserItemManger.h"
#include "Item/InventoryManager.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"

void UEmberInventoryPanel::BP_SetProvider_Implementation()
{
	Super::BP_SetProvider_Implementation();
	
	if (GetOwningPlayer() && GetOwningPlayer()->GetCharacter())
	{
		if (TObjectPtr<UUserItemManger> ItemManger = GetOwningPlayer()->GetCharacter()->GetComponentByClass<UUserItemManger>())
		{
			DataProvider = ItemManger->GetInventoryManager();
		}
	}
}
