// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropItemPanel.h"

#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "Item/Drop/EmberDropItemManager.h"
#include "Item/UserItemManger.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"

void UEmberDropItemPanel::BP_SetProvider_Implementation()
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


FReply UEmberDropItemPanel::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply Reply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

	InitializePanel();

	return Reply.Handled();
}

void UEmberDropItemPanel::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	if (DataProvider)
	{
		if (TObjectPtr<UEmberBaseSlotContainer> ProviderManager = Cast<UEmberBaseSlotContainer>(DataProvider.GetObject()))
		{
			ProviderManager->OnItemChangedDelegate.RemoveDynamic(this, &UEmberDropItemPanel::SlotChanged);
		}
	}
}
