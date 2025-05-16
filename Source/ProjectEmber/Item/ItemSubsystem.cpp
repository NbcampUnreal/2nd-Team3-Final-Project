// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSubsystem.h"
#include "Core/ItemTypes.h"

void UItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("UMyItemSubsystem Initializing... Loading Data Tables."));

    LoadedItemBaseDataTable = LoadDataTable(ItemBaseDataTablePtr, TEXT("ItemBase"));
    LoadedInventoryComponentDataTable = LoadDataTable(InventoryComponentDataTablePtr, TEXT("InventoryComponent"));
    LoadedConsumableComponentDataTable = LoadDataTable(ConsumableComponentDataTablePtr, TEXT("ConsumableComponent"));
    LoadedConsumableEquipmentDataTable = LoadDataTable(EquipmentComponentDataTablePtr, TEXT("EquipmentComponent"));

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
