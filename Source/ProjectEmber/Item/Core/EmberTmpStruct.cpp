// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberTmpStruct.h"
#include "ItemTypes.h"
#include "ItemSystemLibrary.h"
#include "Item/ItemSubsystem.h"



void FEmberTotalSlot::AddIndex(int32 InIndex)
{
	ItemIndexes.Add(InIndex);
}

void FEmberTotalSlot::RemoveIndex(int32 InIndex)
{
	ItemIndexes.Remove(InIndex);
}
