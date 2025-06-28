// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseAutoCreateSlotsPanel.h"

#include "EmberLog/EmberLog.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"

void UEmberBaseAutoCreateSlotsPanel::BP_InitSlots_Implementation()
{
	Super::BP_InitSlots_Implementation();
		
	int32 MaxSlots = IEmberSlotProviderInterface::Execute_GetSlotCount(DataProvider.GetObject());

	Slots.SetNum(MaxSlots);
	for (int32 Index = 0; Index < MaxSlots; Index++)
	{
		if (TObjectPtr<UEmberBaseSlotWidget> NewSlot = CreateWidget<UEmberBaseSlotWidget>(this, SlotClass))
		{
			NewSlot->InitSlot(Index, DataProvider);
			NewSlot->SetPadding(SlotMargin);
			SlotsPanel->AddChild(NewSlot);
			Slots[Index] = NewSlot;
		}
	}
    
}
