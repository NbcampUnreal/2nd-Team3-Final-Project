// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseSlotsPanel.h"

#include "Components/GridPanel.h"
#include "Components/PanelWidget.h"
#include "EmberLog/EmberLog.h"
#include "Item/EmberDataContainer.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"

void UEmberBaseSlotsPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	InitializePanel();
}

void UEmberBaseSlotsPanel::InitializePanel()
{
	BP_SetProvider();
	if (DataProvider)
	{
		BP_CreateSlots();
		BindToManagerDelegates();
	}
	else
	{
		EMBER_LOG(LogEmberItem, Error, TEXT("DataProvider Is Null"));
	}
}

void UEmberBaseSlotsPanel::SlotChanged(int32 InIndex, const FInstancedStruct& InSlotData)
{	
	if (Slots.IsValidIndex(InIndex))
	{
		Slots[InIndex]->SetSlotData(InSlotData);
	}
}

void UEmberBaseSlotsPanel::BP_SetProvider_Implementation()
{
}

void UEmberBaseSlotsPanel::BP_CreateSlots_Implementation()
{
	int32 MaxSlots = IEmberSlotDataProviderInterface::Execute_GetSlotCount(DataProvider.GetObject());
	int32 MaxRow = IEmberSlotDataProviderInterface::Execute_GetSlotMaxRow(DataProvider.GetObject());

	Slots.SetNum(MaxSlots);
	for (int32 Index = 0; Index < MaxSlots; Index++)
	{
		if (TObjectPtr<UEmberBaseSlotWidget> NewSlot = CreateWidget<UEmberBaseSlotWidget>(this, SlotClass))
		{
			NewSlot->InitSlot(Index, DataProvider);
			SlotsPanel->AddChildToGrid(NewSlot, Index / MaxRow, Index % MaxRow);
			Slots[Index] = NewSlot;
		}
	}
	
#if UE_BUILD_DEVELOPMENT
	EMBER_LOG(LogEmberItem, Display, TEXT("Create Slot info : Slots : %d, Row: %d"), MaxSlots, MaxRow);
#endif
    
}

void UEmberBaseSlotsPanel::BindToManagerDelegates_Implementation()
{
	
	if (DataProvider)
	{
		if (TObjectPtr<UEmberDataContainer> ProviderManager = Cast<UEmberDataContainer>(DataProvider.GetObject()))
		{
			ProviderManager->OnDataChangedDelegate.AddDynamic(this, &UEmberBaseSlotsPanel::SlotChanged);
		}
	}
}
