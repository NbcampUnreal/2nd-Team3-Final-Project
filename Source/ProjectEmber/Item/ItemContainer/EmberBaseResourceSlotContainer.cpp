// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseResourceSlotContainer.h"

#include "EmberLog/EmberLog.h"

TMap<FName, int32> UEmberBaseResourceSlotContainer::GetAllItemInfos_Implementation()
{
    TMap<FName, int32> OutItems;

    for (auto& Item : Items)
    {
        if (const FEmberItemEntry* EmberItemEntry = Item.Value.GetPtr<FEmberItemEntry>())
        {
            OutItems.FindOrAdd(EmberItemEntry->ItemID) += EmberItemEntry->Quantity;
        }
    }
    return OutItems;
}

bool UEmberBaseResourceSlotContainer::bConsumeAbleResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems)
{
    for (const FEmberItemEntry& InRequireItem : InRequireItems)
    {
        if (InRequireItem.ItemID.IsValid() && InRequireItem.Quantity > 0)
        {
            if (FInstancedStruct* InstancedStruct = Items.Find(InRequireItem.CreateItemKey()))
            {
                if (const FEmberItemEntry* HasItem = InstancedStruct->GetPtr<FEmberItemEntry>())
                {
                    if (HasItem->Quantity >= InRequireItem.Quantity)
                    {
                        continue;
                    }
                }
            }
        }
        return false;
    }
    return true;
}

int32 UEmberBaseResourceSlotContainer::RemoveItemAutomatic(const FEmberItemEntry& InItem)
{
    if (InItem.Quantity <= 0 || InItem.ItemID.IsNone())
    {
        EMBER_LOG(LogEmberItem, Warning, TEXT("Non Valid InParameter"))
        return 0;
    }

    int32 RemoveIndex = FindRemoveItemIndex(InItem);
    int32 RemovedQuantity = RemoveSlotItemReturnApplied(InItem.Quantity, RemoveIndex);

    while (InItem.Quantity < RemovedQuantity && RemoveIndex < ItemSlots.Num())
    {
        RemoveIndex = FindRemoveItemIndex(InItem);
        RemovedQuantity += RemoveSlotItemReturnApplied(InItem.Quantity, RemoveIndex);
    }
    
    return RemovedQuantity;
}

int32 UEmberBaseResourceSlotContainer::FindRemoveItemIndex(const FEmberItemEntry& InItemEntry)
{
    for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
    {
        FInstancedStruct& InstancedStruct = ItemSlots[Index];
        if (const FEmberItemEntry* ItemEntry = InstancedStruct.GetPtr<FEmberItemEntry>())
        {
            if (ItemEntry->CreateItemKey() == InItemEntry.CreateItemKey())
            {
                return Index;
            }
        }
    }
    return ItemSlots.Num();
}


void UEmberBaseResourceSlotContainer::GetItemInfos_Implementation(TArray<FEmberItemEntry>& InItemEntries,
                                                                  TMap<FEmberItemKey, FInstancedStruct>& OutItemInfos)
{
    for (FEmberItemEntry& ItemInfo : InItemEntries)
    {
        FInstancedStruct& OutInstancedStruct = OutItemInfos.FindOrAdd(ItemInfo.CreateItemKey());
        IEmberResourceProvider::Execute_GetItemInfo(this, ItemInfo, OutInstancedStruct);

    }
}


void UEmberBaseResourceSlotContainer::GetItemInfo_Implementation(FEmberItemEntry& InItemEntry,
    FInstancedStruct& OutItemInfo)
{
    if (InItemEntry.ItemID.IsNone())
    {
        return; 
    }
    
    if (FInstancedStruct* InstancedStruct = Items.Find(InItemEntry.CreateItemKey()))
    {
        if (const FEmberItemEntry* MasterItemData = InstancedStruct->GetPtr<FEmberItemEntry>())
        {
            if (FEmberItemEntry* OutItem = OutItemInfo.GetMutablePtr<FEmberItemEntry>())
            {
                if (OutItem->CreateItemKey() == MasterItemData->CreateItemKey())
                {
                    int32 AddQuantity = MasterItemData->Quantity;
                    OutItem->AddQuantity(AddQuantity);
                    return;
                }
            }
        }
        OutItemInfo = *InstancedStruct;
    }
}

void UEmberBaseResourceSlotContainer::TryConsumeResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems)
{
    if (bConsumeAbleResource_Implementation(InRequireItems))
    {
        for (const FEmberItemEntry& Item : InRequireItems)
        {
            RemoveItemAutomatic(Item);
        }
    }
}

void UEmberBaseResourceSlotContainer::RemoveResourceUntilAble_Implementation(TArray<FEmberItemEntry>& InRequireItems)
{
    for (FEmberItemEntry& RequireItem : InRequireItems)
    {
        int32 RemovedQuantity = RemoveItemAutomatic(RequireItem);
        RequireItem.Quantity -= RemovedQuantity;
    }
    
    InRequireItems.RemoveAll([](const FEmberItemEntry& Item)
        {
            return Item.Quantity <= 0;
        });
}

