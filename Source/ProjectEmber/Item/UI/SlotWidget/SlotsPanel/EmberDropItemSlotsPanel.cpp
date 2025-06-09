// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropItemSlotsPanel.h"

#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "Item/Drop/EmberDropItemManager.h"
#include "Item/UserItemManger.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"


void UEmberDropItemSlotsPanel::BP_SetProvider_Implementation()
{
	Super::BP_SetProvider_Implementation();

	if (GetOwningPlayer() && GetOwningPlayer()->GetCharacter())
	{
		if (TObjectPtr<UUserItemManger> ItemManger = GetOwningPlayer()->GetCharacter()->GetComponentByClass<UUserItemManger>())
		{
			DataProvider = ItemManger->GetEmberDropItemManager();
		}
	}
}


FReply UEmberDropItemSlotsPanel::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply Reply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

	BP_SetProvider_Implementation();
	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		Slots[Index]->InitSlot(Index, DataProvider);
		BindToManagerDelegates_Implementation();
	}
	UpdateSlots();

	return Reply.Handled();

}

void UEmberDropItemSlotsPanel::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	if (DataProvider)
	{
		if (TObjectPtr<UEmberDataContainer> ProviderManager = Cast<UEmberDataContainer>(DataProvider.GetObject()))
		{
			ProviderManager->OnDataChangedDelegate.RemoveDynamic(this, &UEmberBaseSlotsPanel::SlotChanged);
		}
	}
}
