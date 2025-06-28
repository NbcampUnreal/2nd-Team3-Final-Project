// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftWidget.h"

#include "Item/Craft/EmberCraftComponent.h"

void UEmberCraftWidget::InitCraft(UEmberCraftComponent* InCraftComponent, const FCraftSelectInfo& InCraftInfo,
	int32 InIndex)
{
	CraftComponent = InCraftComponent;
	CraftInfo = InCraftInfo;
	CraftIndex = InIndex;
}

void UEmberCraftWidget::StartCraftingItem()
{
	if (CraftComponent)
	{
		CraftComponent->CreateCraftItem(CraftInfo.ResultItemData.ItemID);
	}
}
