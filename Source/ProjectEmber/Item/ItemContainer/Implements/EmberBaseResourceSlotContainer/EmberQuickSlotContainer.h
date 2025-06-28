// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemContainer/EmberBaseSlotContainer.h"
#include "EmberQuickSlotContainer.generated.h"

class UEmberInventorySlotContainer;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberQuickSlotContainer : public UEmberBaseSlotContainer
{
	GENERATED_BODY()
public:

	void ConnectInventorySlotContainer(UEmberInventorySlotContainer* InInventorySlotContainer);

	UFUNCTION()
	void UseSlotItem(int32 InIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	FName SelectQuickSlot(int32 InIndex);
	
	virtual void UseItemInSlot_Implementation(int32 SlotIndex) override;
	virtual void MoveItemByWidget_Implementation(const FGameplayTag& SlotTag, int32 IndexTo, const TScriptInterface<UEmberSlotProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity = -1) override;

	virtual int32 AddSlotItemReturnApplied(const FInstancedStruct& InInstancedStruct, int32 InSlotIndex) override;

	UFUNCTION()
	void InventoryChanged(int32 InIndex, const FInstancedStruct& InSlotData);

	void UpdateRemoveItem();
protected:
	virtual void CreateItemSlot(const FEmberItemEntry& InItemEntry, const int32 InItemIndex) override;
	virtual int32 MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity) override;
	void HandleItemConsumption(const FConsumableInfoRow* ConsumeData);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="QuickSlot")
	int32 CurrentQuickSlotIndex = -1;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UEmberInventorySlotContainer> InventorySlotContainer;
	
};
