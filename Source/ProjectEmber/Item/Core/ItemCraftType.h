// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "ItemCraftType.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FCraftPair
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Craft")
	FDataTableRowHandle ItemData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Craft")
	int32 Quantity{0};
};

USTRUCT(BlueprintType)
struct FCraftInfoRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
	FCraftPair ResultItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
	TArray<FCraftPair> RequireItems;
	
};
