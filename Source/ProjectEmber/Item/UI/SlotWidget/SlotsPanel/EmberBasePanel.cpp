// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBasePanel.h"

#include "EmberLog/EmberLog.h"
#include "Item/ItemContainer/EmberBaseSlotContainer.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"

void UEmberBasePanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	InitializePanel();
}

void UEmberBasePanel::InitializePanel()
{
	BP_SetProvider();
	if (DataProvider)
	{
		BP_InitSlots();
		BindToManagerDelegates();
		UpdateAllSlots();
	}
	else
	{
		EMBER_LOG(LogEmberItem, Error, TEXT("DataProvider Is Null"));
	}
}

void UEmberBasePanel::UnBindToManagerDelegates_Implementation()
{
	if (DataProvider && DataProvider.GetObject())
	{
		if (TObjectPtr<UEmberBaseSlotContainer> ProviderManager = Cast<UEmberBaseSlotContainer>(DataProvider.GetObject()))
		{
			ProviderManager->OnItemChangedDelegate.RemoveDynamic(this, &UEmberBasePanel::SlotChanged);
		}
	}
}

void UEmberBasePanel::BP_InitSlots_Implementation()
{
}

void UEmberBasePanel::SlotChanged_Implementation(int32 InIndex, const FInstancedStruct& InSlotData)
{
	if (Slots.IsValidIndex(InIndex))
	{
		Slots[InIndex]->SetSlotData(InSlotData);
	}
}

void UEmberBasePanel::UpdateAllSlots()
{
	if (DataProvider && Cast<IEmberSlotProviderInterface>(DataProvider.GetObject()))
	{
		for (int32 Index = 0; Index < Slots.Num(); Index++)
		{
			if (Slots.IsValidIndex(Index))
			{
				FInstancedStruct InstancedStruct = IEmberSlotProviderInterface::Execute_GetSlotItemInfo(DataProvider.GetObject(), Index);
				if (Slots[Index])
				{
					Slots[Index]->SetSlotData(InstancedStruct);
				}
			}
		}
	}
}

void UEmberBasePanel::UpdateSlotByIndex(int32 InIndex)
{
	if (DataProvider && Cast<IEmberSlotProviderInterface>(DataProvider.GetObject()))
	{
		if (Slots.IsValidIndex(InIndex))
		{
			FInstancedStruct InstancedStruct = IEmberSlotProviderInterface::Execute_GetSlotItemInfo(DataProvider.GetObject(), InIndex);
			Slots[InIndex]->SetSlotData(InstancedStruct);
		}
	}
}

void UEmberBasePanel::BP_SetProvider_Implementation()
{
}

void UEmberBasePanel::BindToManagerDelegates_Implementation()
{
	UnBindToManagerDelegates();
	if (DataProvider && DataProvider.GetObject())
	{
		if (TObjectPtr<UEmberBaseSlotContainer> ProviderManager = Cast<UEmberBaseSlotContainer>(DataProvider.GetObject()))
		{
			ProviderManager->OnItemChangedDelegate.AddDynamic(this, &UEmberBasePanel::SlotChanged);
		}
	}
}
