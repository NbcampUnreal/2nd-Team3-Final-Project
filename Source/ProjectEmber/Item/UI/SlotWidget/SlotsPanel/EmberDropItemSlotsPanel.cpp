// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropItemSlotsPanel.h"

#include "GameFramework/Character.h"
#include "Item/Drop/EmberDropItemManager.h"
#include "Item/UserItemManger.h"

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
