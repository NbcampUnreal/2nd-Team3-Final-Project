// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberStorageComponent.h"
#include "InventoryManager.h"
#include "Core/EmberTmpStruct.h"


// Sets default values for this component's properties
UEmberStorageComponent::UEmberStorageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	InventoryManager = CreateDefaultSubobject<UInventoryManager>(TEXT("InventoryManager"));
	if (InventoryManager)
	{
		InventoryManager->InitSlot(InventoryMaxSlot, InventoryMaxSlotRow, GetOwner());
	}
	// ...
}


// Called when the game starts
void UEmberStorageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEmberStorageComponent::AddItem(FName ItemID, int32 Quantity, int32 InSlotIndex)
{
	if (InventoryManager)
	{
		FEmberItemEntry Entry = FEmberItemEntry(ItemID, Quantity);
		InventoryManager->AddItem(Entry, InSlotIndex);		
	}
}

TMap<FName, int32> UEmberStorageComponent::GetAllItemInfos_Implementation()
{
	TMap<FName, int32> Items;
	if (InventoryManager)
	{
		Items = IEmberResourceProvider::Execute_GetAllItemInfos(InventoryManager);
	}
	return Items;
}

void UEmberStorageComponent::TryConsumeResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems)
{
	if (InventoryManager)
	{
		IEmberResourceProvider::Execute_TryConsumeResource(InventoryManager, InRequireItems);
	}
}

void UEmberStorageComponent::RemoveResourceUntilAble_Implementation(
	TArray<FEmberItemEntry>& InRequireItems)
{
	if (InventoryManager)
	{
		IEmberResourceProvider::Execute_RemoveResourceUntilAble(InventoryManager, InRequireItems);
	}
}

bool UEmberStorageComponent::bConsumeAbleResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems)
{
	if (InventoryManager)
	{
		return IEmberResourceProvider::Execute_bConsumeAbleResource(InventoryManager, InRequireItems);
	}
	return false;
}


