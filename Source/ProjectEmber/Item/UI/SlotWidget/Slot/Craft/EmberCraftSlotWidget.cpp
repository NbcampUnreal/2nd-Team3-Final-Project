// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftSlotWidget.h"

#include "EmberLog/EmberLog.h"

void UEmberCraftSlotWidget::InitCraftComponent(TScriptInterface<IEmberSlotDataProviderInterface> InDataProvider,
	const FName& InRequestItemID)
{
	DataProvider = InDataProvider;
	RequestItemID = InRequestItemID;

	if (DataProvider)
	{
		FInstancedStruct InstancedStruct = IEmberSlotDataProviderInterface::Execute_GetSlotItemInfo(DataProvider.GetObject(), 0);

		SlotData = FEmberWidgetSlotData(InstancedStruct);

		if (SlotData.ItemID.IsNone())
		{
			FEmberSlotData InSlotData = FEmberSlotData(RequestItemID);
			InSlotData.InitializeInstancedStruct(InstancedStruct);
			SlotData = FEmberWidgetSlotData(InstancedStruct);

		}
	}
}

void UEmberCraftSlotWidget::UpdateCraftSlot()
{
	if (DataProvider)
	{
		FInstancedStruct InstancedStruct = IEmberSlotDataProviderInterface::Execute_GetSlotItemInfo(DataProvider.GetObject(), 0);

		SlotData = FEmberWidgetSlotData(InstancedStruct);

		if (SlotData.ItemID.IsNone())
		{
			FEmberSlotData InSlotData = FEmberSlotData(RequestItemID);
			InSlotData.InitializeInstancedStruct(InstancedStruct);
			SlotData = FEmberWidgetSlotData(InstancedStruct);

		}
	}
}
