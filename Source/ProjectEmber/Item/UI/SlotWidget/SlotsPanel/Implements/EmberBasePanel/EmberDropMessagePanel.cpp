// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropMessagePanel.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/Character.h"
#include "Item/UserItemManger.h"
#include "Item/ItemContainer/Implements/EmberBaseSlotContainer/EmberAddItemMessage.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"

void UEmberDropMessagePanel::BP_SetProvider_Implementation()
{
	Super::BP_SetProvider_Implementation();
	
	if (GetOwningPlayer() && GetOwningPlayer()->GetCharacter())
	{
		if (TObjectPtr<UUserItemManger> ItemManger = GetOwningPlayer()->GetCharacter()->GetComponentByClass<UUserItemManger>())
		{
			DataProvider = ItemManger->GetItemMessageManager();
		}
	}
}
