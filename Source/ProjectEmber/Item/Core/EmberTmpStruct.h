// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberItemStruct.h"
#include "ItemTypes.h"
#include "EmberTmpStruct.generated.h"
/**
 * 
 */
// 아이템 정보 전달체
USTRUCT(BlueprintType)
struct FEmberItemEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item struct")
    FName ItemID = NAME_None;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item struct")
    int32 Quantity = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item struct")
    TArray<FItemEffectApplicationInfo> Enchants = TArray<FItemEffectApplicationInfo>();

    FEmberItemEntry() = default;
    FEmberItemEntry(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>()) : ItemID(InItemID), Quantity(InQuantity), Enchants(InEnchants) {} ;

    void AddQuantity(const int32 InQuantity);
    void RemoveQuantity(const int32 InQuantity);
    FEmberItemKey CreateItemKey() const;
};


USTRUCT(BlueprintType)
struct FEmberMasterItemData
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemID = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemDisplayName = FText();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemDescription = FText();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    TArray<FItemEffectApplicationInfo> EnchantEffects = TArray<FItemEffectApplicationInfo>();

    TSet<FInstancedStruct, FInstancedStructTypeKeyFuncs> DataInstancedStructs;
    
    FEmberMasterItemData() = default;
    virtual ~FEmberMasterItemData() = default;
    
    FEmberMasterItemData(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>());

    virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const
    {
        OutInstancedStruct.InitializeAs<FEmberMasterItemData>(*this);
    }
    
    virtual bool bIsEmpty() const { return ItemID.IsNone() || Quantity <= 0; }
    
    virtual bool bIsFull() const { return Quantity >= ITEM_SYSTEM_MAX_STACK; }
    
    virtual void Clear()
    {
        ItemID = NAME_None;
        Quantity = 0;
        EnchantEffects.Reset();
        DataInstancedStructs.Reset();
    }
    FEmberItemKey CreateItemKey() const;

    void RemoveQuantity(int32& InQuantity);
    void AddQuantity(int32& InQuantity);
};

USTRUCT(BlueprintType)
struct FEmberTotalSlot : public FEmberMasterItemData
{
    GENERATED_BODY()
    FEmberTotalSlot() = default;
    FEmberTotalSlot(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>()) : FEmberMasterItemData(InItemID, InQuantity, InEnchantEffects) {}
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemIndexes")
    TSet<int32> ItemIndexes;
    
    virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
    {
        OutInstancedStruct.InitializeAs<FEmberTotalSlot>(*this);
    }
    void AddIndex(int32 InIndex);
    void RemoveIndex(int32 InIndex);
};


