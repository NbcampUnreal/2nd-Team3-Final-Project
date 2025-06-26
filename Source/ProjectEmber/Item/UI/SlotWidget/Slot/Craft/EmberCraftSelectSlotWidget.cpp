// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftSelectSlotWidget.h"

#include "Item/Craft/EmberCraftComponent.h"

void UEmberCraftSelectSlotWidget::InitCraftItems(const FEmberSlot& InRequestItem)
{
	FInstancedStruct InstancedStruct;
	InRequestItem.InitializeInstancedStruct(InstancedStruct);
	SetSlotData(InstancedStruct);
}

