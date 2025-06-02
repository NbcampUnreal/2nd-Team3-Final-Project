// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropItemManager.h"

#include "EmberLog/EmberLog.h"
#include "Item/ItemSubsystem.h"
#include "Item/Core/ItemSystemLibrary.h"

TArray<FItemPair> UEmberDropItemManager::GetDropItemName(const FName& MonsterID)
{
    TArray<FItemPair> DropItems;

    if (TObjectPtr<UItemSubsystem> ItemSubsystem = UItemSystemLibrary::GetItemSubsystem())
    {
        DropItems = ItemSubsystem->GetDroppedItem(MonsterID);
    }
    
    return DropItems;
}

void UEmberDropItemManager::SetDropItem(const FName& MonsterID)
{
    TArray<FItemPair> DropItemMap = GetDropItemName(MonsterID);
    for (auto& Item : DropItemMap)
    {
        AddItem_Implementation(Item);
    }
}
