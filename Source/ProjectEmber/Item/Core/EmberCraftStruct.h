// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberTmpStruct.h"
#include "ItemCraftType.h"
#include "ItemStruct/EmberSlot.h"
#include "EmberCraftStruct.generated.h"
/**
 * 
 */

struct FCraftPair;
class IEmberResourceProvider;

USTRUCT(BlueprintType)
struct FTotalItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemIndexes")
	int32 TotalQuantity = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemIndexes")
	TSet<int32> ItemIndexes;

	void AddItem(const int32 InQuantity, const int32 InIndex);
	int32 RemoveItemQuantity(const int32 InQuantity);
	void RemoveItemIndexes(const int32 InIndex);
	bool bIsEmpty() const { return TotalQuantity == 0 || ItemIndexes.IsEmpty(); };
};


USTRUCT(BlueprintType)
struct FCraftSelectInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmberSlot ResultItemData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftPair> RequiredItemDataHandles;

	FCraftSelectInfo() = default;
	FCraftSelectInfo(const FCraftInfoRow& InCraftInfoRow);
};
