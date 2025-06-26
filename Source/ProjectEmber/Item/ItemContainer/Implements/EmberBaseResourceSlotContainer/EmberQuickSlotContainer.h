// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemContainer/EmberBaseResourceSlotContainer.h"
#include "Item/ItemContainer/EmberBaseSlotContainer.h"
#include "EmberQuickSlotContainer.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberQuickSlotContainer : public UEmberBaseResourceSlotContainer
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void UseSlotItem(int32 InIndex);

	UFUNCTION(BlueprintCallable, Category = "Item")
	FName SelectQuickSlot(int32 InIndex);
	
	virtual void UseItemInSlot_Implementation(int32 SlotIndex) override;

protected:
	void HandleItemConsumption(const FConsumableInfoRow* ConsumeData);
	virtual void CreateItemSlot(const FEmberItemEntry& InItemEntry, const int32 InItemIndex) override;
	virtual int32 MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="QuickSlot")
	int32 CurrentQuickSlotIndex = -1;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
	
};
