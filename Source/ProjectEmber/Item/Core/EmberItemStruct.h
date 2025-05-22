// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "EmberItemStruct.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FEmberSlotData
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

    FEmberSlotData() = default;
    virtual ~FEmberSlotData() = default;

    FEmberSlotData(const FName& InItemID, const int32 InQuantity = 0);

    virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const
    {
        OutInstancedStruct.InitializeAs<FEmberSlotData>(*this);
    }
    
    virtual bool bIsEmpty() const { return ItemID.IsNone() || Quantity <= 0; }
    
    virtual bool bIsFull() const { return Quantity >= MaxStackSize; }
    
    virtual void Clear()
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
struct FEquipmentSlotData : public FEmberSlotData
{
    GENERATED_BODY()

    TOptional<FEquipmentInfoRow> EquipmentInfo;

    FEquipmentSlotData() = default;
    
    FEquipmentSlotData(const FEmberSlotData& InInventorySlotData);
    FEquipmentSlotData(const FName& InItemID, const int32 InQuantity);

    virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
    {
        OutInstancedStruct.InitializeAs<FEquipmentSlotData>(*this);
    }
    
    virtual void Clear() override
    {
        Super::Clear();
        EquipmentInfo = FEquipmentInfoRow();
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

    FEmberItemInfo(const FEmberSlotData& InItemID);
    
    bool IsEmpty() const { return ItemID.IsNone();}
    
    void Clear()
    {
        ItemID = NAME_None;
        ItemTags.Reset();
        ActiveEffects.Empty();
    }
};

USTRUCT(BlueprintType)
struct FItemPair
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item struct")
    FName ItemID = NAME_None;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item struct")
    int32 Quantity = 0;

    FItemPair() = default;
    FItemPair(const FName& InItemID, const int32 InQuantity) : ItemID(InItemID), Quantity(InQuantity) {};
};




