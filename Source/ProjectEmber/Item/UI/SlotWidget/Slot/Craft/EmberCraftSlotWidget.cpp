// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftSlotWidget.h"

#include "EmberLog/EmberLog.h"
#include "Item/Craft/EmberCraftComponent.h"
#include "Item/ItemContainer/Implements/EmberBaseResourceSlotContainer/EmberCraftContainer.h"
#include "Item/UI/SlotWidget/SlotsPanel/EmberBasePanel.h"


void UEmberCraftSlotWidget::InitCraftComponent(TScriptInterface<IEmberSlotProviderInterface> InDataProvider,
                                               int32 InSlotIndex)
{
	UnBindToManagerDelegates();
	
	DataProvider = InDataProvider;
	SlotIndex = InSlotIndex;
	
	if (DataProvider)
	{
		if (UEmberCraftComponent* CraftComponent = Cast<UEmberCraftComponent>(DataProvider.GetObject()))
		{
			if (UEmberCraftContainer* CraftContainer = CraftComponent->GetSlotContainer())
			{
				CraftContainer->OnItemChangedDelegate.AddDynamic(this, &UEmberCraftSlotWidget::SlotChanged);
				SetSlotData(CraftComponent->ResultItemInfo(SlotIndex));
			}
		}
	}
}

void UEmberCraftSlotWidget::UnBindToManagerDelegates()
{
	if (DataProvider)
	{
		if (UEmberCraftComponent* CraftComponent = Cast<UEmberCraftComponent>(DataProvider.GetObject()))
		{
			if (UEmberCraftContainer* CraftContainer = CraftComponent->GetSlotContainer())
			{
				CraftContainer->OnItemChangedDelegate.RemoveDynamic(this, &UEmberCraftSlotWidget::SlotChanged);
			}
		}
	}
}

void UEmberCraftSlotWidget::SlotChanged(int32 InIndex, const FInstancedStruct& InSlotData)
{
	if (SlotIndex == InIndex)
	{
		SetSlotData(InSlotData);
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
