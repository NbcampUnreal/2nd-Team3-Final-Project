// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftSlot.h"

FEmberCraftSlot::FEmberCraftSlot(const FCraftInfoRow& CraftTable)
{
	*this = FEmberCraftSlot(CraftTable.ResultItem.ItemData.RowName, CraftTable.ResultItem.Quantity);

	for (const FCraftPair& RequireItem : CraftTable.RequireItems)
	{
		RequiredItems.Add(FEmberItemEntry(RequireItem.ItemData.RowName, RequireItem.Quantity));
	}
}
