// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftComponent.h"

#include "EmberLog/EmberLog.h"
#include "Item/UserItemManger.h"
#include "Item/Core/EmberTmpStruct.h"
#include "Item/Core/ItemCraftType.h"
#include "Item/Core/ItemStruct/EmberSlot.h"
#include "Item/ItemContainer/EmberBaseResourceSlotContainer.h"
#include "Item/ItemContainer/Implements/EmberBaseResourceSlotContainer/EmberCraftContainer.h"


// Sets default values for this component's properties
UEmberCraftComponent::UEmberCraftComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	SlotContainer = CreateDefaultSubobject<UEmberCraftContainer>(TEXT("CraftManager"));
	if (SlotContainer)
	{
		SlotContainer->InitOwner(GetOwner());
	}
}

void UEmberCraftComponent::BeginPlay()
{
	Super::BeginPlay();

	if (CraftDataTable)
	{
		CraftItems.Reset();
		CraftDataTable->GetAllRows<FCraftInfoRow>(TEXT("CraftRow"), CraftItems);
		
		if (SlotContainer)
		{
			SlotContainer->InitSlotCount(CraftItems.Num(), 0);
		}

		for (int32 Index = 0; Index < CraftItems.Num(); ++Index)
		{
			if (CraftItems[Index])
			{
				FEmberItemEntry ItemEntry = FEmberItemEntry(CraftItems[Index]->ResultItem.ItemData.RowName);
				SlotContainer->AddSlotItemReturnApplied(ItemEntry, Index);
			}
		}
	}
}

void UEmberCraftComponent::CreateCraftItem(const FName& InItemID)
{
	if (bAbleCrafting(InItemID))
	{
		FInstancedStruct InstancedStruct = CraftItemBase(InItemID);
		
		if (SlotContainer)
		{
			for (int32 Index = 0; Index < CraftItems.Num(); ++Index)
			{

				if (CraftItems[Index]->ResultItem.ItemData.RowName == InItemID)
				{
					int32 QuantityToAdd = SlotContainer->AddSlotItemReturnApplied(InstancedStruct, Index);
				}
			}
		}
	}
}

FInstancedStruct UEmberCraftComponent::GetSlotItemInfo_Implementation(int32 InIndex) const
{
	return IEmberSlotProviderInterface::Execute_GetSlotItemInfo(SlotContainer, InIndex);
}

void UEmberCraftComponent::AddItemByWidget_Implementation(FEmberItemEntry& InOutItem, int32 InSlotIndex)
{
	IEmberSlotProviderInterface::Execute_AddItemByWidget(SlotContainer, InOutItem, InSlotIndex);
}

int32 UEmberCraftComponent::RemoveItemFromSlot_Implementation(int32 InSlotIndex, int32 QuantityToRemove)
{
	return IEmberSlotProviderInterface::Execute_RemoveItemFromSlot(SlotContainer, InSlotIndex, QuantityToRemove);
}

void UEmberCraftComponent::MoveItemByWidget_Implementation(const FGameplayTag& SlotTag, int32 IndexTo,
	const TScriptInterface<UEmberSlotProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity)
{
	return IEmberSlotProviderInterface::Execute_MoveItemByWidget(SlotContainer, SlotTag, IndexTo, AnotherProvider, IndexFrom, Quantity);

}

int32 UEmberCraftComponent::GetSlotMaxRow_Implementation() const
{
	return IEmberSlotProviderInterface::Execute_GetSlotMaxRow(SlotContainer);
}

void UEmberCraftComponent::UseItemInSlot_Implementation(int32 SlotIndex)
{
	return IEmberSlotProviderInterface::Execute_UseItemInSlot(SlotContainer, SlotIndex);

}

bool UEmberCraftComponent::bAbleCrafting(const FName& InItemID)
{
	for (int32 Index = 0; Index < CraftItems.Num(); ++Index)
	{
		if (CraftItems[Index]->ResultItem.ItemData.RowName == InItemID)
		{
			FInstancedStruct InstancedStruct = IEmberSlotProviderInterface::Execute_GetSlotItemInfo(SlotContainer, Index);
			
			if (const FEmberSlot* EmberItemEntry = InstancedStruct.GetPtr<FEmberSlot>())
			{
				if (EmberItemEntry->bAbleAddQuantity(CraftItems[Index]->ResultItem.Quantity))
				{
					return true;
				}
			}
		}
	}
	return false;
}

TArray<FCraftInfoRow> UEmberCraftComponent::BP_CraftItemInfos() const
{
	TArray<FCraftInfoRow> CopyCraftItems;
	for (auto& CraftItem : CraftItems)
	{
		CopyCraftItems.Add(*CraftItem);
	}
	return CopyCraftItems;
}

TArray<FCraftInfoRow*> UEmberCraftComponent::CraftItemInfos() const
{
	return CraftItems;
}

FCraftInfoRow* UEmberCraftComponent::CraftItemInfo(const int32 InIndex) const
{
	if (CraftItems.IsValidIndex(InIndex))
	{
		return CraftItems[InIndex];
	}
	return nullptr;
}

FInstancedStruct UEmberCraftComponent::ResultItemInfo(const int32 InIndex) const
{
	return SlotContainer->GetInstancedItemSlotInfo(InIndex);
}

TObjectPtr<UEmberCraftContainer> UEmberCraftComponent::GetSlotContainer() const
{
	return SlotContainer;
}

int32 UEmberCraftComponent::GetSlotCount_Implementation() const
{
	return IEmberSlotProviderInterface::Execute_GetSlotCount(SlotContainer);
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
		
		FEmberSlot ResultItem;
		
		void GetItemInfos(TArray<FEmberItemEntry>& InItemEntry, TMap<FEmberItemKey, FInstancedStruct>& OutItemInfos);

		if (!CraftInfoRow->ResultItem.ItemData.IsNull())
		{
			ResultItem = FEmberSlot(CraftInfoRow->ResultItem.ItemData.RowName, CraftInfoRow->ResultItem.Quantity);
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
			ResultItem.InitializeInstancedStruct(ReturnItemInfo);
		}
	}
	
	return ReturnItemInfo;
}

void UEmberCraftComponent::RemoveStoredItem(int32& InQuantity)
{
}
