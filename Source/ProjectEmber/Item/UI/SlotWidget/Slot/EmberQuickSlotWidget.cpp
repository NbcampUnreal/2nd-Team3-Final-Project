// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberQuickSlotWidget.h"

#include "EmberLog/EmberLog.h"

void UEmberQuickSlotWidget::UseQuickSlotItem(int32 InIndex) const
{
	EMBER_LOG(LogEmberItem, Display, TEXT("Start"));
	IEmberSlotDataProviderInterface::Execute_UseItemInSlot(DataProvider.GetObject(), InIndex);

}
