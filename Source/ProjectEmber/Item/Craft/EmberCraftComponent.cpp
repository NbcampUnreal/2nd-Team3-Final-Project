// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftComponent.h"

#include "EmberLog/EmberLog.h"
#include "Item/UserItemManger.h"
#include "Item/Core/EmberTmpStruct.h"
#include "Item/Core/ItemCraftType.h"


// Sets default values for this component's properties
UEmberCraftComponent::UEmberCraftComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

void UEmberCraftComponent::CreateCraftItem(const FName& InItemID)
{
	FInstancedStruct InstancedStruct = CraftItemBase(InItemID);

	if (const FEmberSlotData* CreateSlotData = InstancedStruct.GetPtr<FEmberSlotData>())
	{
		if (StoredItem.ItemID == CreateSlotData->ItemID)
		{
			int32 AddQuantity = StoredItem.Quantity;
			StoredItem.AddQuantity(AddQuantity);
		}
		else
		{
			// 나중에 드랍으로
			RemoveStoredItem(StoredItem.Quantity);
			StoredItem = *CreateSlotData;
		}

	}
}

FEmberSlotData UEmberCraftComponent::GetStoredItem()
{
	return StoredItem;
}

bool UEmberCraftComponent::bAbleCrafting(const FName& InItemID)
{
	if (StoredItem.bIsFull())
	{
		return false;
	}
	return true;
}

TArray<FCraftInfoRow> UEmberCraftComponent::CraftItemInfos() const
{
	TArray<FCraftInfoRow> CraftItemInfos;
	if (CraftDataTable)
	{
		TArray<FCraftInfoRow*> CraftItems;
		CraftDataTable->GetAllRows<FCraftInfoRow>(TEXT("CraftRow"), CraftItems);

		for (FCraftInfoRow* DataTableRow : CraftItems)
		{
			if (DataTableRow)
			{
				CraftItemInfos.Add(*DataTableRow);
			}
		}
	}
	return CraftItemInfos;

}

FName UEmberCraftComponent::GetSlotItemID_Implementation(int32 InIndex) const
{
	return StoredItem.ItemID;
}

FInstancedStruct UEmberCraftComponent::GetSlotItemInfo_Implementation(int32 InIndex) const
{
	FInstancedStruct InstancedStruct;
	StoredItem.InitializeInstancedStruct(InstancedStruct);
	return InstancedStruct;
}

int32 UEmberCraftComponent::RemoveItemFromSlot_Implementation(int32 SlotIndex, int32 QuantityToRemove)
{
	int32 RemoveQuantity = StoredItem.Quantity;
	
	RemoveStoredItem(RemoveQuantity);
	return RemoveQuantity;
}

FInstancedStruct UEmberCraftComponent::CraftItemBase(const FName& InItemID)
{
	FInstancedStruct ReturnItemInfo;

	if (!CraftDataTable || !InItemID.IsValid() || InItemID.IsNone())
	{
		return ReturnItemInfo;
	}

	if (const FCraftInfoRow* CraftInfoRow = CraftDataTable->FindRow<FCraftInfoRow>(InItemID, TEXT("CraftInfo")))
	{
		
		TMap<FEmberItemKey, FInstancedStruct> RequireItemsResult;
		TArray<FEmberItemEntry> RequireItems;
		
		FEmberSlotData RequestItem;
		
		void GetItemInfos(TArray<FEmberItemEntry>& InItemEntry, TMap<FEmberItemKey, FInstancedStruct>& OutItemInfos);

		if (!CraftInfoRow->RequestItem.ItemData.IsNull())
		{
			RequestItem = FEmberSlotData(CraftInfoRow->RequestItem.ItemData.RowName, CraftInfoRow->RequestItem.Quantity);
		}
		for (const FCraftPair& RequireItem : CraftInfoRow->RequireItems)
		{
			if (!RequireItem.ItemData.IsNull())
			{
				RequireItems.Add(FEmberItemEntry(RequireItem.ItemData.RowName, RequireItem.Quantity));
			}
		}

		if (IEmberResourceProvider::Execute_bConsumeAbleResource(this, RequireItems))
		{
			IEmberResourceProvider::Execute_TryConsumeResource(this, RequireItems);
			RequestItem.InitializeInstancedStruct(ReturnItemInfo);
		}
	}
	
	return ReturnItemInfo;
}

void UEmberCraftComponent::RemoveStoredItem(int32& InQuantity)
{
	StoredItem.RemoveQuantity(InQuantity);
}
