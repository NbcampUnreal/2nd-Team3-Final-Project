// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/EmberItemStruct.h"
#include "UObject/Interface.h"
#include "EmberSlotDataProviderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UEmberSlotDataProviderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTEMBER_API IEmberSlotDataProviderInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "SlotProvider")
	virtual int32 GetSlotCount() const = 0;
	
	UFUNCTION(BlueprintCallable, Category = "SlotProvider")
	virtual int32 AddItemAndHandleOverflow(FName ItemIDToAdd, int32 QuantityToAdd, FVector DropLocation, FRotator DropRotation) = 0;

	UFUNCTION(BlueprintCallable, Category = "SlotProvider")
	virtual int32 RemoveItemFromSlot(int32 SlotIndex, int32 QuantityToRemove = 0) = 0;

	UFUNCTION(BlueprintCallable, Category = "SlotProvider")
	virtual void UseItemInSlot(int32 SlotIndex) = 0;

	UFUNCTION(BlueprintCallable, Category = "SlotProvider")
	virtual void MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity) = 0;

};
