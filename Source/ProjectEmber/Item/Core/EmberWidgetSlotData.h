// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "EmberWidgetSlotData.generated.h"
/**
 * 
 */

struct FItemEffectApplicationInfo;

USTRUCT(BlueprintType)
struct FEmberWidgetSlotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot")
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot", meta = (ClampMin = "0"))
	int32 Quantity = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	int32 MaxStackSize = 1;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
	FText ItemDisplayName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
	FText ItemDescription = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
	TSoftObjectPtr<UTexture2D> ItemIcon = nullptr;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
	TArray<FItemEffectApplicationInfo> Enchants = TArray<FItemEffectApplicationInfo>();
    
	FEmberWidgetSlotData() = default;

	FEmberWidgetSlotData(const FInstancedStruct& InSlotData);
    
	bool IsEmpty() const { return ItemID.IsNone() || Quantity <= 0; }
    
	void Clear()
	{
		ItemID = NAME_None;
		Quantity = 0;
		MaxStackSize = 1;
		ItemDisplayName = FText();
		ItemDescription = FText();
		ItemIcon = nullptr;
		Enchants.Empty();
	}
};
