// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/EmberItemStruct.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "EmberBaseItemContainer.generated.h"

/**
 * 
 */


UCLASS()
class PROJECTEMBER_API UEmberBaseItemContainer : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual int32 AddItem(const FEmberItemEntry& InItem);
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual int32 RemoveItem(const FEmberItemEntry& InItem);
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual FInstancedStruct GetItemInfo(const FName& InItemID);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", SaveGame) // SaveGame 필요시 추가
	TMap<FEmberItemKey, FInstancedStruct> Data;

};
