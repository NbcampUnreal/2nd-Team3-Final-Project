// Fill out your copyright notice in the Description page of Project Settings.


#include "UserItemManger.h"

#include "InventoryManager.h"
#include "QuickSlotManager.h"
#include "Item/Drop/EmberDropItemManager.h"
#include "EmberEquipmentManager.h"
#include "Character/EmberCharacter.h"
#include "Core/EmberTmpStruct.h"
#include "Core/ItemStruct/Implements/EmberSlot/EmberInventorySlot.h"
#include "Core/ItemStruct/Implements/EmberSlot/EmberQuickSlot.h"
#include "Craft/EmberCraftComponent.h"
#include "EmberLog/EmberLog.h"
#include "ItemContainer/Implements/EmberBaseSlotContainer/EmberAddItemMessage.h"
#include "Kismet/GameplayStatics.h"


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
		InventoryManager->InitOwner(GetOwner());
		InventoryManager->InitSlotCount(InventoryMaxSlot, InventoryMaxSlotRow);
	}
	QuickSlotManager = CreateDefaultSubobject<UQuickSlotManager>(TEXT("QuickSlotManager"));
	if (QuickSlotManager)
	{
		QuickSlotManager->InitOwner(GetOwner());
		QuickSlotManager->InitSlotCount(QuickMaxSlot, QuickMaxSlotRow);
		QuickSlotManager->ConnectInventorySlotContainer(InventoryManager);

	}

	DropItemManager = CreateDefaultSubobject<UEmberDropItemManager>(TEXT("DropItemManager"));
	if (DropItemManager)
	{
		DropItemManager->InitOwner(GetOwner());
		DropItemManager->InitSlotCount(DropItemMaxSlot, DropItemMaxSlotRow);
	}
	
	EquipmentManager = CreateDefaultSubobject<UEmberEquipmentManager>(TEXT("EquipmentManager"));

	if (EquipmentManager)
	{
		EquipmentManager->InitOwner(GetOwner());

	}
	ItemMessageManager = CreateDefaultSubobject<UEmberAddItemMessage>(TEXT("ItemMessageManager"));

	if (ItemMessageManager)
	{
		ItemMessageManager->InitOwner(GetOwner());
		ItemMessageManager->InitSlotCount(5, 1);
	}
	CraftComponent = CreateDefaultSubobject<UEmberCraftComponent>(TEXT("CraftComponent"));
	// ...
}


// Called when the game starts
void UUserItemManger::BeginPlay()
{
	Super::BeginPlay();

	
	// ...

	if (ItemMessageManager)
	{
		ItemMessageManager->StartMessage();
	}
}

void UUserItemManger::InitAbilitySystem()
{
	if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(GetOwner()))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = EmberCharacter->GetAbilitySystemComponent())
		{
			if (EquipmentManager)
			{
				EquipmentManager->SetOwnerAbilitySystemComponent(AbilitySystemComponent);
			}
			if (InventoryManager)
			{
				InventoryManager->SetOwnerAbilitySystemComponent(AbilitySystemComponent);
			}
			if (QuickSlotManager)
			{
				QuickSlotManager->SetOwnerAbilitySystemComponent(AbilitySystemComponent);
			}
		}
	}
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

FEmberQuickSlot UUserItemManger::GetQuickSlotInfo(int32 InIndex)
{
	if (const FEmberQuickSlot* Item = QuickSlotManager->GetSlotItemInfo(InIndex).GetPtr<FEmberQuickSlot>())
	{
		return FEmberQuickSlot(*Item);
	}
	return FEmberQuickSlot();
}

void UUserItemManger::UseQuickSlot(int32 InIndex)
{
	QuickSlotManager->UseSlotItem(InIndex);
}

FEmberInventorySlot UUserItemManger::GetInventorySlotInfo(int32 InIndex)
{
	if (const FEmberInventorySlot* Item = InventoryManager->GetSlotItemInfo(InIndex).GetPtr<FEmberInventorySlot>())
	{
		return FEmberInventorySlot(*Item);
	}
	return FEmberInventorySlot();
}

void UUserItemManger::UseInventorySlotInfo(int32 InIndex)
{
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

void UUserItemManger::SaveInventoryItem(TArray<FEmberItemEntry>& InOutItem)
{
	if (InventoryManager)
	{
		InventoryManager->EmberSave(InOutItem);
	}
}

void UUserItemManger::LoadInventoryItem(TArray<FEmberItemEntry>& InItem)
{
	if (InventoryManager)
	{
		InventoryManager->EmberLoad(InItem);
	}
}

void UUserItemManger::SaveEquipmentItem(TArray<FEmberItemEntry>& InOutItem)
{
	if (EquipmentManager)
	{
		EquipmentManager->EmberSave(InOutItem);
	}
}

void UUserItemManger::LoadEquipmentItem(TArray<FEmberItemEntry>& InItem)
{
	if (EquipmentManager)
	{
		EquipmentManager->EmberLoad(InItem);
	}
}

void UUserItemManger::SaveQuickSlotItem(TArray<FEmberItemEntry>& InOutItem)
{
	if (QuickSlotManager)
	{
		QuickSlotManager->EmberSave(InOutItem);
	}
}

void UUserItemManger::LoadQuickSlotItem(TArray<FEmberItemEntry>& InItem)
{
	if (QuickSlotManager)
	{
		QuickSlotManager->EmberLoad(InItem);
	}
}

void UUserItemManger::AddItem(FName ItemID, int32 Quantity, int32 InSlotIndex)
{
	FEmberItemEntry Entry = FEmberItemEntry(ItemID, Quantity);
	InventoryManager->AddSlotItem(Entry, InSlotIndex);
}

void UUserItemManger::AddItem(const FInstancedStruct& InInstancedStruct, int32 InSlotIndex)
{
	if (InventoryManager)
	{
		InventoryManager->AddSlotItemReturnApplied(InInstancedStruct, InSlotIndex);
	}
}

void UUserItemManger::AddItemAndAlarm(FName ItemID, int32 Quantity, int32 InSlotIndex)
{

	FEmberItemEntry Entry = FEmberItemEntry(ItemID, Quantity);
	AddItemAndAlarm(Entry.CreateInstancedStruct(), InSlotIndex);
}

void UUserItemManger::AddItemAndAlarm(const FInstancedStruct& InInstancedStruct, int32 InSlotIndex)
{
	AddItem(InInstancedStruct, InSlotIndex);

	if (IsValid(ItemMessageManager))
	{
		ItemMessageManager->AddSlotItemReturnApplied(InInstancedStruct, InSlotIndex);
	}
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

UEmberAddItemMessage* UUserItemManger::GetItemMessageManager()
{
	return ItemMessageManager;
}

TMap<FName, int32> UUserItemManger::GetAllItemInfos_Implementation()
{
	TMap<FName, int32> Items;

	if (InventoryManager)
	{
		Items = IEmberResourceProvider::Execute_GetAllItemInfos(InventoryManager);
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
	if (EquipmentManager)
	{
		IEmberResourceProvider::Execute_RemoveResourceUntilAble(EquipmentManager, InRequireItems);
	}
}

FName UUserItemManger::SelectQuickSlot(const int32 InIndex) const
{
	return QuickSlotManager->SelectQuickSlot(InIndex);
}

