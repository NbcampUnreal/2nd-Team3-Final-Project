// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftUserWidget.h"
#include "Character/EmberCharacter.h"
#include "EmberLog/EmberLog.h"
#include "Item/UserItemManger.h"
#include "Item/Craft/EmberCraftComponent.h"


void UEmberCraftUserWidget::SetCraftComponent(AEmberCharacter* InEmberCharacter)
{
	EmberCharacter = InEmberCharacter;
}

void UEmberCraftUserWidget::CrateItem(const FName& ItemName)
{
	if (EmberCharacter)
	{
		FItemPair Item = EmberCharacter->GetCraftComponent()->CraftItem(ItemName);
		EmberCharacter->GetItemManager()->AddItem(Item.ItemID, Item.Quantity);
	}
}
