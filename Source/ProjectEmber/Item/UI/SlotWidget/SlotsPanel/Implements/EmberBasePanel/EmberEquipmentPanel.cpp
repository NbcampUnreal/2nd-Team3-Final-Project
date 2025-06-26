// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberEquipmentPanel.h"

#include "GameFramework/Character.h"
#include "Item/EmberEquipmentManager.h"
#include "Item/UserItemManger.h"

void UEmberEquipmentPanel::BP_SetProvider_Implementation()
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

int32 UEmberEquipmentPanel::GetSlotIndex(const FGameplayTag& InTag) const
{
	if (UEmberEquipmentContainer* EquipmentContainer = Cast<UEmberEquipmentContainer>(DataProvider.GetObject()))
	{
		return EquipmentContainer->GetSlotIndex(InTag);
	}
	return -1;
}
