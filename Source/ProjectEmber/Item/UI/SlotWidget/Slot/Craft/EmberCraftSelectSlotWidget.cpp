// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftSelectSlotWidget.h"

#include "Item/Craft/EmberCraftComponent.h"

void UEmberCraftSelectSlotWidget::InitCraftItems(const FEmberMasterItemData& InRequestItem)
{
	FInstancedStruct InstancedStruct;
	FEmberSlotData InSlotData = FEmberSlotData(InRequestItem.ItemID, InRequestItem.Quantity);
	InSlotData.InitializeInstancedStruct(InstancedStruct);
	SetSlotData(InstancedStruct);
}

