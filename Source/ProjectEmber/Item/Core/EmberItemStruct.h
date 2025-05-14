// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
        SlotData.Reset();
    }
};
