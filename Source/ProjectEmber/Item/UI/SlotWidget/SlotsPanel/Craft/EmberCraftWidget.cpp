// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftWidget.h"

#include "Item/Craft/EmberCraftComponent.h"

void UEmberCraftWidget::InitCraft(UEmberCraftComponent* InCraftComponent, const FCraftSelectInfo& InCraftInfo)
{
	CraftComponent = InCraftComponent;
	CraftInfo = InCraftInfo;
}

void UEmberCraftWidget::StartCraftingItem()
{
	if (CraftComponent)
	{
		CraftComponent->CreateCraftItem(CraftInfo.RequestItemData.ItemID);
	}
}
