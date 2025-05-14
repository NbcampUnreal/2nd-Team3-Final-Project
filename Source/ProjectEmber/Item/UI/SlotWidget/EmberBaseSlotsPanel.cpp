// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseSlotsPanel.h"

#include "EmberBaseSlotWidget.h"
#include "Components/PanelWidget.h"

void UEmberBaseSlotsPanel::InitializePanel(TScriptInterface<IEmberSlotDataProviderInterface> InDataProvider)
{
	DataProvider = InDataProvider;
	if (DataProvider)
	{
		BP_BindToManagerDelegates();
		BP_CreateAndArrageSlots();
	}
}
