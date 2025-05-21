// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlotManager.h"

FName UQuickSlotManager::SelectQuickSlot(const int32 InIndex)
{
	if (CurrentQuickSlotIndex == InIndex)
	{
		CurrentQuickSlotIndex = -1;
		return FName();
	}
	else
	{
		CurrentQuickSlotIndex = InIndex;
		return GetSlotItemID_Implementation(CurrentQuickSlotIndex);
	}
}
