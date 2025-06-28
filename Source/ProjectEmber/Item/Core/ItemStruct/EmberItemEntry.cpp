// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberItemEntry.h"

#include "EmberLog/EmberLog.h"


FEmberItemEntry::FEmberItemEntry(const FInstancedStruct& InInstancedStruct)
{
	if (const FEmberItemEntry* BaseItem = InInstancedStruct.GetPtr<FEmberItemEntry>())
	{
		*this = *BaseItem;
	}
}

void FEmberItemEntry::AddQuantity(int32& InOutQuantity)
{
	int32 AbleAddQuantity = FMath::Min(MaxQuantity - Quantity, InOutQuantity);
	Quantity += AbleAddQuantity;
	InOutQuantity -= AbleAddQuantity;
}

void FEmberItemEntry::RemoveQuantity(int32& InOutQuantity)
{
	int32 AbleAddQuantity = FMath::Min(Quantity, InOutQuantity);
	Quantity -= AbleAddQuantity;
	InOutQuantity -= AbleAddQuantity;
}

FInstancedStruct FEmberItemEntry::CreateInstancedStruct() const
{
	FInstancedStruct InstancedStruct = FInstancedStruct();
	this->InitializeInstancedStruct(InstancedStruct);
	return InstancedStruct;
}

bool FEmberItemEntry::bIsEmpty() const
{
	return ItemID.IsNone() || Quantity <= 0; 
}

bool FEmberItemEntry::bIsFull() const
{
	return Quantity >= MaxQuantity; 
}

bool FEmberItemEntry::bAbleAddQuantity(const int32 InQuantity) const
{
	return Quantity + InQuantity <= MaxQuantity; 
}

bool FEmberItemEntry::bAbleRemoveQuantity(const int32 InQuantity) const
{
	return Quantity - InQuantity >= 0; 
}

int32 FEmberItemEntry::AddQuantityReturnApplied(const int32 InQuantity)
{
	int32 AbleAddQuantity = FMath::Min(MaxQuantity - Quantity, InQuantity);
	Quantity += AbleAddQuantity;
	return AbleAddQuantity;
}

int32 FEmberItemEntry::RemoveQuantityReturnApplied(const int32 InQuantity)
{
	int32 AbleAddQuantity = FMath::Min(Quantity, InQuantity);
	Quantity -= AbleAddQuantity;
	return AbleAddQuantity;
}

FEmberItemKey FEmberItemEntry::CreateItemKey() const
{
	return FEmberItemKey(ItemID, Enchants);
}
