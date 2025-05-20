// Fill out your copyright notice in the Description page of Project Settings.


#include "UserItemManger.h"

#include "InventoryManager.h"
#include "QuickSlotManager.h"
#include "EmberDropItemManager.h"
#include "EmberEquipmentManager.h"


// Sets default values for this component's properties
UUserItemManger::UUserItemManger()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	if (!GetOwner())
	{
		return;
	}
	
	InventoryManager = CreateDefaultSubobject<UInventoryManager>(TEXT("InventoryManager"));
	if (InventoryManager)
	{
		InventoryManager->InitSlot(InventoryMaxSlot, InventoryMaxSlotRow, GetOwner());
	}
	QuickSlotManager = CreateDefaultSubobject<UQuickSlotManager>(TEXT("QuickSlotManager"));
	if (QuickSlotManager)
	{
		QuickSlotManager->InitSlot(QuickMaxSlot, QuickMaxSlotRow, GetOwner());
	}

	DropItemManager = CreateDefaultSubobject<UEmberDropItemManager>(TEXT("DropItemManager"));
	if (DropItemManager)
	{
		DropItemManager->InitSlot(DropItemMaxSlot, DropItemMaxSlotRow, GetOwner());
	}
	
	EquipmentManager = CreateDefaultSubobject<UEmberEquipmentManager>(TEXT("EquipmentManager"));

	if (EquipmentManager)
	{
		EquipmentManager->InitSlot(4, 0, GetOwner());
	}

	
	// ...
}


// Called when the game starts
void UUserItemManger::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UUserItemManger::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FEmberItemInfo UUserItemManger::GetQuickSlotInfo(int32 InIndex)
{
	if (const FEmberItemInfo* Item = QuickSlotManager->GetSlotDataByIndex(InIndex).GetPtr<FEmberItemInfo>())
	{
		return FEmberItemInfo(*Item);
	}
	return FEmberItemInfo();
}

void UUserItemManger::UseQuickSlotInfo(int32 InIndex)
{
	QuickSlotManager->UseItemInSlot_Implementation(InIndex);
}

FEmberItemInfo UUserItemManger::GetInventorySlotInfo(int32 InIndex)
{
	if (const FEmberItemInfo* Item = InventoryManager->GetSlotDataByIndex(InIndex).GetPtr<FEmberItemInfo>())
	{
		return FEmberItemInfo(*Item);
	}
	return FEmberItemInfo();
}

void UUserItemManger::UseInventorySlotInfo(int32 InIndex)
{
	InventoryManager->UseItemInSlot_Implementation(InIndex);
}

void UUserItemManger::AddItem(FName ItemID, int32 Quantity, int32 InSlotIndex)
{
	InventoryManager->AddItem_Implementation(FItemPair(ItemID, Quantity), InSlotIndex);
}

const UInventoryManager* UUserItemManger::GetInventoryManager() const
{
	return InventoryManager;
}

UInventoryManager* UUserItemManger::GetInventoryManager()
{
	return InventoryManager;
}

const UQuickSlotManager* UUserItemManger::GetQuickSlotManager() const
{
	return QuickSlotManager;
}

UQuickSlotManager* UUserItemManger::GetQuickSlotManager()
{
	return QuickSlotManager;
}

const UEmberEquipmentManager* UUserItemManger::GetEquipmentManager() const
{
	return EquipmentManager;
}

UEmberEquipmentManager* UUserItemManger::GetEquipmentManager()
{
	return EquipmentManager;
}

const class UEmberDropItemManager* UUserItemManger::GetEmberDropItemManager() const
{
	return DropItemManager;
}

UEmberDropItemManager* UUserItemManger::GetEmberDropItemManager()
{
	return DropItemManager;
}

TArray<FItemPair> UUserItemManger::GetItems_Implementation()
{
	TArray<FItemPair> Items;
	for (const FInstancedStruct& SlotInstance : InventoryManager->GetInventorySlots())
	{
		if (const FEmberSlotData* SlotData = SlotInstance.GetPtr<FEmberSlotData>())
		{
			if (!SlotData->bIsEmpty())
			{
				FItemPair Item;
		
				Item.ItemID = SlotData->ItemID;
				Item.Quantity = SlotData->Quantity;

				Items.Add(Item);
			}
		}
	}

	return Items;
}

bool UUserItemManger::bTryConsumeResource_Implementation(const TArray<FItemPair>& RequireItems)
{
	return true;
}
