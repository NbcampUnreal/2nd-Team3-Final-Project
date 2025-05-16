// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseSlotsPanel.h"

#include "Components/PanelWidget.h"
#include "EmberLog/EmberLog.h"

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
		EMBER_LOG(LogTemp, Warning, TEXT("DataProvider Is Null"));
	}
}

void UEmberBaseSlotsPanel::BP_SetProvider_Implementation()
{
}

void UEmberBaseSlotsPanel::BP_CreateSlots_Implementation()
{
}

void UEmberBaseSlotsPanel::BindToManagerDelegates_Implementation()
{
}
