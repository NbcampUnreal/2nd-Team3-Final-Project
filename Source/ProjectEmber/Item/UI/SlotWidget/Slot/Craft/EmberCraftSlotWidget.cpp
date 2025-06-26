// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftSlotWidget.h"

#include "EmberLog/EmberLog.h"
#include "Item/Craft/EmberCraftComponent.h"
#include "Item/ItemContainer/Implements/EmberBaseResourceSlotContainer/EmberCraftContainer.h"


void UEmberCraftSlotWidget::InitCraftComponent(TScriptInterface<IEmberSlotProviderInterface> InDataProvider,
	int32 InSlotIndex)
{
	DataProvider = InDataProvider;
	SlotIndex = InSlotIndex;
	
	if (DataProvider)
	{
		if (UEmberCraftComponent* CraftComponent = Cast<UEmberCraftComponent>(DataProvider.GetObject()))
		{
			SetSlotData(CraftComponent->ResultItemInfo(SlotIndex));
		}
	}
}

void UEmberCraftSlotWidget::UpdateSlot()
{
	if (DataProvider)
	{
		if (UEmberCraftComponent* CraftComponent = Cast<UEmberCraftComponent>(DataProvider.GetObject()))
		{
			SlotData = FEmberWidgetSlot(CraftComponent->ResultItemInfo(SlotIndex));
		}
	}
	
	Super::UpdateSlot();
}
