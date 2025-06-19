// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberResourceSlot.h"

#include "Item/Core/EmberTmpStruct.h"
#include "Item/Craft/EmberCraftComponent.h"

void UEmberResourceSlot::InitCraft(UEmberCraftComponent* InCraftComponent, const FCraftPair& InRequestInfo)
{
	CraftComponent = InCraftComponent;
	
	if (CraftComponent)
	{
		RequiredQuantity = InRequestInfo.Quantity;
		FEmberItemEntry ItemEntry = FEmberItemEntry(InRequestInfo.ItemData.RowName);
		FInstancedStruct OutInstancedStruct;
		IEmberResourceProvider::Execute_GetItemInfo(CraftComponent, ItemEntry, OutInstancedStruct);
		FEmberSlotData RequestInfo = FEmberSlotData(InRequestInfo.ItemData.RowName);
		if (const FEmberMasterItemData* ItemData = OutInstancedStruct.GetPtr<FEmberMasterItemData>())
		{
			RequestInfo.Quantity = ItemData->Quantity;
		}
		RequestInfo.InitializeInstancedStruct(OutInstancedStruct);
		SlotData = FEmberWidgetSlotData(OutInstancedStruct);
	}

	UpdateSlot();
}

float UEmberResourceSlot::GetPercentage()
{
	float Percentage = FMath::Min(SlotData.Quantity / RequiredQuantity, 1.0f);
	return Percentage;
}

void UEmberResourceSlot::UpdateCraftSlot()
{
	if (CraftComponent)
	{
		FEmberItemEntry ItemEntry = FEmberItemEntry(SlotData.ItemID);
		FInstancedStruct OutInstancedStruct;
		IEmberResourceProvider::Execute_GetItemInfo(CraftComponent, ItemEntry, OutInstancedStruct);
		FEmberSlotData RequestInfo = FEmberSlotData(SlotData.ItemID);
		if (const FEmberMasterItemData* ItemData = OutInstancedStruct.GetPtr<FEmberMasterItemData>())
		{
			RequestInfo.Quantity = ItemData->Quantity;
		}
		RequestInfo.InitializeInstancedStruct(OutInstancedStruct);
		SlotData = FEmberWidgetSlotData(OutInstancedStruct);
	}
}
