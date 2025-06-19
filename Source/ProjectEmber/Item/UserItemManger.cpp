// Fill out your copyright notice in the Description page of Project Settings.


#include "UserItemManger.h"

#include "InventoryManager.h"
#include "QuickSlotManager.h"
#include "Item/Drop/EmberDropItemManager.h"
#include "EmberEquipmentManager.h"
#include "Core/EmberTmpStruct.h"
#include "Craft/EmberCraftComponent.h"
#include "EmberLog/EmberLog.h"


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

	CraftComponent = CreateDefaultSubobject<UEmberCraftComponent>(TEXT("CraftComponent"));
	// ...
}


// Called when the game starts
void UUserItemManger::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UUserItemManger::InitAbilitySystem()
{
	
	if (InventoryManager)
	{
		InventoryManager->InitOwner(GetOwner());
	}
	if (QuickSlotManager)
	{
		QuickSlotManager->InitOwner(GetOwner());
	}
	if (DropItemManager)
	{
		DropItemManager->InitOwner(GetOwner());
	}
	if (EquipmentManager)
	{
		EquipmentManager->InitOwner(GetOwner());
	}
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

void UUserItemManger::UseQuickSlot(int32 InIndex)
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

void UUserItemManger::SetDropProvider(UEmberDropItemManager* InDropItemProvider)
{
	DropItemManager = InDropItemProvider;
}

void UUserItemManger::ClearDropProvider()
{
	DropItemManager = nullptr;
}

FEmberMasterItemData UUserItemManger::DebugGetItemInfo(const FName& InSlotName)
{
	FInstancedStruct ItemData;
	FEmberItemEntry ItemEntry = FEmberItemEntry(InSlotName);
	IEmberResourceProvider::Execute_GetItemInfo(this, ItemEntry, ItemData);
	if (const FEmberMasterItemData* Data = ItemData.GetPtr<FEmberMasterItemData>())
	{
		return *Data;
	}
		
	return FEmberMasterItemData();
}


void UUserItemManger::AddItem(FName ItemID, int32 Quantity, int32 InSlotIndex)
{
	FEmberItemEntry Entry = FEmberItemEntry(ItemID, Quantity);
	InventoryManager->AddItem(Entry, InSlotIndex);
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

void UUserItemManger::GetItemInfo_Implementation(FEmberItemEntry& InItemEntry,
	FInstancedStruct& OutItemInfo)
{
	if (InventoryManager)
	{
		IEmberResourceProvider::Execute_GetItemInfo(InventoryManager, InItemEntry, OutItemInfo);
	}
	if (QuickSlotManager)
	{
		IEmberResourceProvider::Execute_GetItemInfo(QuickSlotManager, InItemEntry, OutItemInfo);
	}
	if (EquipmentManager)
	{
		IEmberResourceProvider::Execute_GetItemInfo(EquipmentManager, InItemEntry, OutItemInfo);
	}
}

void UUserItemManger::GetItemInfos_Implementation(TArray<FEmberItemEntry>& InItemEntries,
	TMap<FEmberItemKey, FInstancedStruct>& OutItemInfos)
{
	if (InventoryManager && InItemEntries.Num() > 0)
	{
		IEmberResourceProvider::Execute_GetItemInfos(InventoryManager, InItemEntries, OutItemInfos);
	}
	if (IsValid(QuickSlotManager) && InItemEntries.Num() > 0)
	{
		IEmberResourceProvider::Execute_GetItemInfos(QuickSlotManager, InItemEntries, OutItemInfos);
	}
	if (EquipmentManager && InItemEntries.Num() > 0)
	{
		IEmberResourceProvider::Execute_GetItemInfos(EquipmentManager, InItemEntries, OutItemInfos);
	}
}

void UUserItemManger::TryConsumeResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems)
{

	if (bConsumeAbleResource_Implementation(InRequireItems))
	{
		TArray<FEmberItemEntry> RequireItems = InRequireItems;
		if (InventoryManager)
		{
			IEmberResourceProvider::Execute_RemoveResourceUntilAble(InventoryManager, RequireItems);
		}
		if (QuickSlotManager)
		{
			IEmberResourceProvider::Execute_RemoveResourceUntilAble(QuickSlotManager, RequireItems);
		}
		if (EquipmentManager)
		{
			IEmberResourceProvider::Execute_RemoveResourceUntilAble(EquipmentManager, RequireItems);
		}
	}
}

bool UUserItemManger::bConsumeAbleResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems)
{
	TMap<FName, int32> Items = GetAllItemInfos();
	for (const FEmberItemEntry& RequireItem : InRequireItems)
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

void UUserItemManger::RemoveResourceUntilAble_Implementation(TArray<FEmberItemEntry>& InRequireItems)
{
	if (InventoryManager)
	{
		IEmberResourceProvider::Execute_RemoveResourceUntilAble(InventoryManager, InRequireItems);
	}
	if (QuickSlotManager)
	{
		IEmberResourceProvider::Execute_RemoveResourceUntilAble(QuickSlotManager, InRequireItems);
	}
	if (EquipmentManager)
	{
		IEmberResourceProvider::Execute_RemoveResourceUntilAble(EquipmentManager, InRequireItems);
	}
}

FName UUserItemManger::SelectQuickSlot(const int32 InIndex) const
{
	return QuickSlotManager->SelectQuickSlot(InIndex);
}

