// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/EmberDropStruct.h"
#include "Core/EmberItemStruct.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "ItemSubsystem.generated.h"

struct FItemMasterInfoRow;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class PROJECTEMBER_API UItemSubsystem : public UGameInstanceSubsystem
{
     GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    const FItemMasterInfoRow* GetItemMasterInfoRow(FName ItemID) const;

	TArray<FItemPair> GetDroppedItem(FName MonsterID);
protected:
	bool SelectWeightedItem(const TArray<FEmberDropItemGroup>& ItemsToSelectFrom, FEmberDropItemGroup& OutSelectedItem);
	TArray<FItemEffectApplicationInfo> SetEnchantEquipment(const TObjectPtr<UDataTable>& EnchantDataTable);
protected:

    UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables", meta = (AllowedClasses = "/Script/Engine.DataTable", DisplayName = "Item Base Data Table"))
    TSoftObjectPtr<UDataTable> ItemBaseDataTablePtr;

    UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables", meta = (AllowedClasses = "/Script/Engine.DataTable", DisplayName = "Inventory Component Data Table"))
    TSoftObjectPtr<UDataTable> InventoryComponentDataTablePtr;

    UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables", meta = (AllowedClasses = "/Script/Engine.DataTable", DisplayName = "Consumable Component Data Table"))
    TSoftObjectPtr<UDataTable> ConsumableComponentDataTablePtr;

    UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables", meta = (AllowedClasses = "/Script/Engine.DataTable", DisplayName = "Consumable Component Data Table"))
    TSoftObjectPtr<UDataTable> EquipmentComponentDataTablePtr;

    UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables", meta = (AllowedClasses = "/Script/Engine.DataTable", DisplayName = "Consumable Component Data Table"))
    TSoftObjectPtr<UDataTable> MonsterLootDataTablePtr;

    UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables", meta = (AllowedClasses = "/Script/Engine.DataTable", DisplayName = "Consumable Component Data Table"))
    TSoftObjectPtr<UDataTable> LootPoolDataTablePtr;

    UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables", meta = (AllowedClasses = "/Script/Engine.DataTable", DisplayName = "Consumable Component Data Table"))
    TSoftObjectPtr<UDataTable> EnchantDataTablePtr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item DataTables")
    TObjectPtr<UDataTable> LoadedItemBaseDataTable = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item DataTables")
    TObjectPtr<UDataTable> LoadedInventoryComponentDataTable = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item DataTables")
    TObjectPtr<UDataTable> LoadedConsumableComponentDataTable = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item DataTables")
    TObjectPtr<UDataTable> LoadedConsumableEquipmentDataTable = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item DataTables")
    TObjectPtr<UDataTable> LoadedMonsterLootDataTable = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item DataTables")
    TObjectPtr<UDataTable> LoadedLootPoolDataTable = nullptr;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item DataTables")
    TObjectPtr<UDataTable> EnchantDataTable = nullptr;
    
    /** 데이터 테이블 로드를 위한 내부 헬퍼 함수 */
    TObjectPtr<UDataTable> LoadDataTable(const TSoftObjectPtr<UDataTable>& DataTablePtr, const FName& TableIdentifier) const;
};
