// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberItemStruct.h"
#include "ItemTypes.h"
#include "ItemStruct/EmberMasterItemData.h"
#include "EmberTmpStruct.generated.h"
/**
 * 
 */

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


