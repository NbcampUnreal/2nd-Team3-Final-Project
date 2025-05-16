// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberInventorySlotsPanel.h"

#include "Components/GridPanel.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "Item/InventoryManagerComponent.h"
#include "Item/UserItemManger.h"
#include "Item/UI/SlotWidget/EmberSlotDataProviderInterface.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"

void UEmberInventorySlotsPanel::BP_CreateSlots_Implementation()
{
	Super::BP_CreateSlots_Implementation();

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
}

void UEmberInventorySlotsPanel::BP_SetProvider_Implementation()
{
	Super::BP_SetProvider_Implementation();
	
	if (GetOwningPlayer() && GetOwningPlayer()->GetCharacter())
	{
		if (TObjectPtr<UUserItemManger> ItemManger = GetOwningPlayer()->GetCharacter()->GetComponentByClass<UUserItemManger>())
		{
			DataProvider = ItemManger->GetInventoryManager();
		}
	}
}

void UEmberInventorySlotsPanel::BindToManagerDelegates_Implementation()
{
	Super::BindToManagerDelegates_Implementation();

	if (DataProvider)
	{
		if (TObjectPtr<UInventoryManagerComponent> InventoryManagerComponent = Cast<UInventoryManagerComponent>(DataProvider.GetObject()))
		{
			InventoryManagerComponent->OnInventoryChanged.AddDynamic(this, &UEmberInventorySlotsPanel::SlotChanged);
		}
	}
}

void UEmberInventorySlotsPanel::SlotChanged(int32 InIndex, const FInventorySlotData& InSlotData)
{
	if (Slots.IsValidIndex(InIndex))
	{
		EMBER_LOG(LogTemp, Warning, TEXT("DataProvider %d"), InIndex);

		Slots[InIndex]->SetSlotData(InSlotData);
	}
}
