// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "EmberSlotDataProviderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	int32 GetSlotCount() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	int32 GetSlotMaxRow() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	FName GetSlotItemID(int32 InIndex) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	FGameplayTag GetSlotType() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	int32 AddItemAndHandleOverflow(FName ItemIDToAdd, int32 QuantityToAdd, FVector DropLocation, FRotator DropRotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	int32 RemoveItemFromSlot(int32 SlotIndex, int32 QuantityToRemove);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	void UseItemInSlot(int32 SlotIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SlotProvider")
	void MoveItemBySlot(const FGameplayTag& SlotTag, int32 IndexTo, const TScriptInterface<UEmberSlotDataProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity);
	
};
