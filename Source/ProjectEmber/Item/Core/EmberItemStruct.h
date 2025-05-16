// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "EmberItemStruct.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FInventorySlotData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot")
    FName ItemID = NAME_None;

    // 해당 슬롯에 있는 아이템의 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot", meta = (ClampMin = "0"))
    int32 Quantity = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
    int32 MaxStackSize = 1;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
    FText ItemDisplayName = FText();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
    FText ItemDescription = FText();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
    TOptional<FDataTableRowHandle> ConsumableData;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
    TOptional<FDataTableRowHandle> EquipmentData;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
    TOptional<FDataTableRowHandle> SlotData;

    FInventorySlotData() = default;

    FInventorySlotData(const FName& InItemID);
    
    bool IsEmpty() const { return ItemID.IsNone() || Quantity <= 0; }
    
    void Clear()
    {
        ItemID = NAME_None;
        Quantity = 0;
        MaxStackSize = 1;
        ConsumableData.Reset();
        EquipmentData.Reset();
        SlotData.Reset();
    }
};


USTRUCT(BlueprintType)
struct FQuickSlotData
{
    GENERATED_BODY()

    int32 InventorySlotIndex = 0;

    FInventorySlotData InventorySlotData;

    FQuickSlotData() = default;
    FQuickSlotData(int32 InInventorySlotIndex, const FInventorySlotData& InInventorySlotData);
    
    bool IsEmpty() const { return InventorySlotData.ItemID.IsNone() || InventorySlotData.Quantity <= 0; }
    
    void Clear()
    {
        InventorySlotData.Clear();
    }
};

USTRUCT(BlueprintType)
struct FEmberItemInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
    FGameplayTagContainer ItemTags;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
    TArray<FItemEffectApplicationInfo> ActiveEffects;
	
    FEmberItemInfo() = default;

    FEmberItemInfo(const FInventorySlotData& InItemID);
    
    bool IsEmpty() const { return ItemID.IsNone();}
    
    void Clear()
    {
        ItemID = NAME_None;
        ItemTags.Reset();
        ActiveEffects.Empty();
    }
};
