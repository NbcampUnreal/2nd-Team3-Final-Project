// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSubsystem.h"

#include "AbilitySystemComponent.h"
#include "Ability/EmberItemAttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Core/EmberDropStruct.h"
#include "Core/EmberItemDeveloperSetting.h"
#include "Core/ItemTypes.h"
#include "EmberLog/EmberLog.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SlotWidget/ItemDetailWidget.h"
#include "UI/SlotWidget/SlotsPanel/Implements/EmberBasePanel/EmberQuickSlotPanel.h"

void UItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("UMyItemSubsystem Initializing... Loading Data Tables."));

    LoadedItemBaseDataTable = LoadDataTable(ItemBaseDataTablePtr, TEXT("ItemBase"));
    LoadedInventoryComponentDataTable = LoadDataTable(InventoryComponentDataTablePtr, TEXT("InventoryDataTable"));
    LoadedConsumableComponentDataTable = LoadDataTable(ConsumableComponentDataTablePtr, TEXT("ConsumableDataTable"));
    LoadedConsumableEquipmentDataTable = LoadDataTable(EquipmentComponentDataTablePtr, TEXT("EquipmentDataTable"));
    LoadedMonsterLootDataTable = LoadDataTable(MonsterLootDataTablePtr, TEXT("MonsterLootDataTable"));
    LoadedLootPoolDataTable = LoadDataTable(LootPoolDataTablePtr, TEXT("LootPoolDataTablePtr"));
    EnchantDataTable = LoadDataTable(EnchantDataTablePtr, TEXT("EnchantDataTablePtr"));

}

void UItemSubsystem::Deinitialize()
{
    LoadedItemBaseDataTable = nullptr;
    LoadedInventoryComponentDataTable = nullptr;
    LoadedConsumableComponentDataTable = nullptr;
    LoadedConsumableEquipmentDataTable = nullptr;

    UE_LOG(LogTemp, Log, TEXT("UMyItemSubsystem Deinitializing."));
    Super::Deinitialize();
}

TObjectPtr<UDataTable> UItemSubsystem::LoadDataTable(const TSoftObjectPtr<UDataTable>& DataTablePtr, const FName& TableIdentifier) const
{
    if (DataTablePtr.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("Data Table path for '%s' is not set in ItemSubsystem config."), *TableIdentifier.ToString());
        return nullptr;
    }

    UDataTable* LoadedTable = DataTablePtr.LoadSynchronous();
    if (LoadedTable)
    {
        UE_LOG(LogTemp, Log, TEXT("Data Table '%s' loaded successfully: %s"), *TableIdentifier.ToString(), *DataTablePtr.ToString());
        // 로드 후 유효성 검사 (예: RowStruct가 null이 아닌지 등)를 추가할 수 있음
        if (LoadedTable->GetRowStruct() == nullptr)
        {
             UE_LOG(LogTemp, Error, TEXT("Data Table '%s' (%s) loaded but has no Row Struct defined!"), *TableIdentifier.ToString(), *DataTablePtr.ToString());
             return nullptr; // 유효하지 않은 테이블로 간주
        }
    }
    else
    {
         UE_LOG(LogTemp, Error, TEXT("Failed to load Data Table '%s' from path: %s"), *TableIdentifier.ToString(), *DataTablePtr.ToString());
    }
    return LoadedTable;
}

const FItemMasterInfoRow* UItemSubsystem::GetItemMasterInfoRow(FName ItemID) const
{
    if (LoadedItemBaseDataTable)
    {
        return LoadedItemBaseDataTable->FindRow<FItemMasterInfoRow>(ItemID, TEXT("GetItemBaseData"));
    }
    UE_LOG(LogTemp, Error, TEXT("Failed to load LoadedItemBaseDataTable"));
    return nullptr;
        
}

TArray<FItemPair> UItemSubsystem::GetDroppedItem(FName MonsterID, const UAbilitySystemComponent* AbilitySystemComponent)
{
    TArray<FItemPair> FinalDroppedItems;

    if (!LoadedMonsterLootDataTable || !LoadedLootPoolDataTable || !LoadedItemBaseDataTable)
    {
        EMBER_LOG(LogEmberItem, Error, TEXT("CalculateMonsterDrops: Invalid DataTable provided."));
        return FinalDroppedItems;
    }

    const FString ContextString(TEXT("LoadedMonsterLootDataTable"));
    FMonsterLootTableRow* MonsterLootData = LoadedMonsterLootDataTable->FindRow<FMonsterLootTableRow>(MonsterID, ContextString);

    if (!MonsterLootData)
    {
        EMBER_LOG(LogEmberItem, Warning, TEXT("CalculateMonsterDrops: MonsterLootRowName '%s' not found in LoadedMonsterLootDataTable."), *MonsterID.ToString());
        return FinalDroppedItems;
    }

    float DropRate = 1.0f;
    int32 DropBonus = 0;
    if (AbilitySystemComponent)
    {
        if (const UEmberItemAttributeSet* ItemAttributeSet = Cast<UEmberItemAttributeSet>(AbilitySystemComponent->GetAttributeSet(UEmberItemAttributeSet::StaticClass())))
        {
            DropRate += ItemAttributeSet->GetItemDropRate();
            DropBonus += ItemAttributeSet->GetItemDropBonus();
        }
    }
    for (const FEmberDropEntry& DropEntry : MonsterLootData->EmberDropEntries)
    {
        if (FMath::FRand() * DropRate <= DropEntry.DropChance)
        {
            int32 ActualQuantityOrPicks = FMath::RandRange(DropEntry.MinQuantity, DropEntry.MaxQuantity);
            if (ActualQuantityOrPicks <= 0) continue;
            ActualQuantityOrPicks += DropBonus;
            if (DropEntry.bIsDropPool)
            {
                if (!DropEntry.ItemID.DataTable || !DropEntry.ItemID.RowName.IsValid() || DropEntry.ItemID.RowName.IsNone())
                {
                     EMBER_LOG(LogEmberItem, Warning, TEXT("CalculateMonsterDrops: Invalid TargetID for Loot Pool in DropEntry."));
                     continue;
                }
                
                FPoolContents* PoolToProcess = LoadedLootPoolDataTable->FindRow<FPoolContents>(DropEntry.ItemID.RowName, ContextString);
                
                if (!PoolToProcess || PoolToProcess->PossibleItems.IsEmpty())
                {
                    EMBER_LOG(LogEmberItem, Warning, TEXT("CalculateMonsterDrops: Loot Pool '%s' not found or empty in LootPoolDataTable."), *DropEntry.ItemID.RowName.ToString());
                    continue;
                }

                for (int32 PickCount = 0; PickCount < ActualQuantityOrPicks; ++PickCount)
                {
                    FEmberDropItemGroup SelectedPoolItem;
                    if (SelectWeightedItem(PoolToProcess->PossibleItems, SelectedPoolItem))
                    {
                        int32 QuantityOfSelectedItem = FMath::RandRange(SelectedPoolItem.MinQuantityIfPicked, SelectedPoolItem.MaxQuantityIfPicked);
                        if (QuantityOfSelectedItem > 0)
                        {
                            FItemPair Item = FItemPair(SelectedPoolItem.ItemID.RowName, QuantityOfSelectedItem);
                            if (DropEntry.EnchantTable)
                            {
                                Item.Enchants = SetEnchantEquipment(DropEntry.EnchantTable);
                            }
                            FinalDroppedItems.Add(Item);
                        }
                    }
                }
            }
            else
            {
                if (!DropEntry.ItemID.DataTable || !DropEntry.ItemID.RowName.IsValid() || DropEntry.ItemID.RowName.IsNone())
                {
                     EMBER_LOG(LogEmberItem, Warning, TEXT("CalculateMonsterDrops: Invalid TargetID for Direct Item in DropEntry."));
                     continue;
                }
                
                FItemPair Item = FItemPair(DropEntry.ItemID.RowName, ActualQuantityOrPicks);
                if (DropEntry.EnchantTable)
                {
                    Item.Enchants = SetEnchantEquipment(DropEntry.EnchantTable);
                }
                FinalDroppedItems.Add(Item);
            }

        }
    }

    return FinalDroppedItems;
}

TObjectPtr<UItemDetailWidget> UItemSubsystem::GetItemDetailWidget()
{
    if (!DetailWidget)
    {
        if (const UEmberItemDeveloperSetting* Setting = UEmberItemDeveloperSetting::Get())
        {
            if (UClass* InDetailWidgetClass = Setting->ItemDetailWidgetClass)
            {
                DetailWidget = CreateWidget<UItemDetailWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), InDetailWidgetClass, TEXT("DetailWidget"));
            }
        }
    }
    if (DetailWidget)
    {
        DetailWidget->AddToViewport(100);
        DetailWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    return DetailWidget;
}

UEmberQuickSlotPanel* UItemSubsystem::BP_GetQuickSlotPanel()
{
    return GetQuickSlotWidget();
}

TObjectPtr<UEmberQuickSlotPanel> UItemSubsystem::GetQuickSlotWidget()
{
    if (!QuickSlotWidget)
    {
        if (const UEmberItemDeveloperSetting* ItemSetting = UEmberItemDeveloperSetting::Get())
        {
            QuickSlotWidget = CreateWidget<UEmberQuickSlotPanel>(UGameplayStatics::GetPlayerController(GetWorld(), 0), ItemSetting->QuickSlotWidgetClass);

        }
    }
    if (QuickSlotWidget)
    {
        QuickSlotWidget->AddToViewport(99);
    }
    return QuickSlotWidget;
}


bool UItemSubsystem::SelectWeightedItem(const TArray<FEmberDropItemGroup>& ItemsToSelectFrom,
                                        FEmberDropItemGroup& OutSelectedItem)
{
    if (ItemsToSelectFrom.IsEmpty())
    {
        return false;
    }

    int32 TotalWeight = 0;
    for (const FEmberDropItemGroup& Item : ItemsToSelectFrom)
    {
        if (Item.Weight > 0)
        {
            TotalWeight += Item.Weight;
        }
    }

    if (TotalWeight <= 0)
    {
        return false;
    }

    int32 RandomWeightValue = FMath::RandRange(0, TotalWeight - 1);

    int32 CurrentWeightSum = 0;
    for (const FEmberDropItemGroup& Item : ItemsToSelectFrom)
    {
        if (Item.Weight <= 0) continue;

        CurrentWeightSum += Item.Weight;
        if (RandomWeightValue < CurrentWeightSum)
        {
            OutSelectedItem = Item;
            return true;
        }
    }

    if (!ItemsToSelectFrom.IsEmpty())
    {
        for (int32 Index = ItemsToSelectFrom.Num() - 1; Index >= 0; --Index) {
            if (ItemsToSelectFrom[Index].Weight > 0) {
                OutSelectedItem = ItemsToSelectFrom[Index];
                return true;
            }
        }
    }
    return false;
}

TArray<FItemEffectApplicationInfo> UItemSubsystem::SetEnchantEquipment(const TObjectPtr<UDataTable>& InEnchantDataTable)
{
    if (!InEnchantDataTable)
    {
        return TArray<FItemEffectApplicationInfo>();
    }
    TArray<FItemEffectApplicationInfo> OutEnchants;

    int32 EnchantCount = FMath::RandRange(0, 2);
    TArray<FItemEffectApplicationInfo*> Enchants;
    InEnchantDataTable->GetAllRows<FItemEffectApplicationInfo>(TEXT("Enchant"), Enchants);

    for (int32 Index = 0; Index < EnchantCount; ++Index)
    {
        if (Enchants.IsValidIndex(Index))
        {
            int32 SwapIndex = FMath::RandRange(Index, Enchants.Num() - 1);
            Enchants.Swap(Index, SwapIndex);
            OutEnchants.Add(*Enchants[Index]);
        }
    }

    return OutEnchants;
}

