// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberDropStruct.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FEmberDropEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DropInfoRow")
	FDataTableRowHandle ItemID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DropInfoRow")
	bool bIsDropPool = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DropInfoRow")
	TObjectPtr<UDataTable> EnchantTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DropChance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	int32 MinQuantity = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	int32 MaxQuantity = 1;
};


USTRUCT(BlueprintType)
struct FMonsterLootTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	FName MonsterID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	TArray<FEmberDropEntry> EmberDropEntries;
};

USTRUCT(BlueprintType)
struct FEmberDropItemGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PoolItem")
	FDataTableRowHandle ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PoolItem", meta = (ClampMin = "1"))
	int32 Weight = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PoolItem", meta = (ClampMin = "1"))
	int32 MinQuantityIfPicked = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PoolItem", meta = (ClampMin = "1"))
	int32 MaxQuantityIfPicked = 1;
};


USTRUCT(BlueprintType)
struct FPoolContents : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootPool")
	TArray<FEmberDropItemGroup> PossibleItems;
};

