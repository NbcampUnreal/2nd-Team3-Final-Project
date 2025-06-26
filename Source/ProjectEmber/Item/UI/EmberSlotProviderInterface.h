// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/ItemStruct/EmberItemEntry.h"
#include "UObject/Interface.h"
#include "EmberSlotProviderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UEmberSlotProviderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTEMBER_API IEmberSlotProviderInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.:
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	int32 GetSlotCount() const;
		
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	int32 GetSlotMaxRow() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	FInstancedStruct GetSlotItemInfo(int32 InIndex) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	void AddItemByWidget(FEmberItemEntry& InOutItem, int32 InSlotIndex = -1);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	int32 RemoveItemFromSlot(int32 InSlotIndex, int32 QuantityToRemove);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	void UseItemInSlot(int32 SlotIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	void MoveItemByWidget(const FGameplayTag& SlotTag, int32 IndexTo, const TScriptInterface<UEmberSlotProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity);
};
