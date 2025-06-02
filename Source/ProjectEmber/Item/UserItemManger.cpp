// Fill out your copyright notice in the Description page of Project Settings.


#include "UserItemManger.h"

#include "InventoryManager.h"
#include "QuickSlotManager.h"
#include "Item/Drop/EmberDropItemManager.h"
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

void UUserItemManger::GetMonsterItems(const FName MonsterID)
{
	DropItemManager->SetDropItem(MonsterID);
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
TMap<FName, int32> UUserItemManger::GetAllItemInfos_Implementation()
{
	TMap<FName, int32> Items;

	if (InventoryManager)
	{
		Items = IEmberResourceProvider::Execute_GetAllItemInfos(InventoryManager);
	}
	if (QuickSlotManager)
	{
		TMap<FName, int32> ProviderItems = IEmberResourceProvider::Execute_GetAllItemInfos(QuickSlotManager);
		for (auto& ProviderItem : ProviderItems)
		{
			if (int32* ItemQuantity = Items.Find(ProviderItem.Key))
			{
				*ItemQuantity += ProviderItem.Value;
			}
			else
			{
				Items.Emplace(ProviderItem.Key, ProviderItem.Value);
			}
		}
	}
	if (EquipmentManager)
	{
		TMap<FName, int32> ProviderItems = EquipmentManager->IEmberResourceProvider::Execute_GetAllItemInfos(EquipmentManager);
		for (auto& ProviderItem : ProviderItems)
		{
			if (int32* ItemQuantity = Items.Find(ProviderItem.Key))
			{
				*ItemQuantity += ProviderItem.Value;
			}
			else
			{
				Items.Emplace(ProviderItem.Key, ProviderItem.Value);
			}
		}
	}
	return Items;
}

void UUserItemManger::TryConsumeResource_Implementation(const TArray<FItemPair>& InRequireItems)
{

	if (bConsumeAbleResource_Implementation(InRequireItems))
	{
		TArray<FItemPair> RequireItems = InRequireItems;
		if (InventoryManager)
		{
			RequireItems = InventoryManager->RemoveResourceUntilAble(RequireItems);
		}
		if (QuickSlotManager)
		{
			RequireItems = QuickSlotManager->RemoveResourceUntilAble(RequireItems);
		}
		if (EquipmentManager)
		{
			RequireItems = EquipmentManager->RemoveResourceUntilAble(RequireItems);
		}
	}
}

bool UUserItemManger::bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems)
{
	TMap<FName, int32> Items = GetAllItemInfos();
	for (const FItemPair& RequireItem : InRequireItems)
	{
		if (RequireItem.ItemID.IsValid() && RequireItem.Quantity > 0)
		{
			int32* HasQuantity = Items.Find(RequireItem.ItemID);
			if (!HasQuantity || *HasQuantity < RequireItem.Quantity)
			{
				return false;
			}
		}
	}
    
	return true;
}

TArray<FItemPair> UUserItemManger::RemoveResourceUntilAble_Implementation(const TArray<FItemPair>& InRequireItems)
{
	TArray<FItemPair> RequireItems = InRequireItems;
	if (InventoryManager)
	{
		RequireItems = IEmberResourceProvider::Execute_RemoveResourceUntilAble(InventoryManager, RequireItems);
	}
	if (QuickSlotManager)
	{
		RequireItems = IEmberResourceProvider::Execute_RemoveResourceUntilAble(QuickSlotManager, RequireItems);
	}
	if (EquipmentManager)
	{
		RequireItems = IEmberResourceProvider::Execute_RemoveResourceUntilAble(EquipmentManager, RequireItems);
	}
	return RequireItems;
}

FName UUserItemManger::SelectQuickSlot(const int32 InIndex) const
{
	return QuickSlotManager->SelectQuickSlot(InIndex);
}

