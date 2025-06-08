// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberDataContainer.h"
#include "EmberEquipmentManager.generated.h"


UCLASS(ClassGroup=(Custom))
class PROJECTEMBER_API UEmberEquipmentManager : public UEmberDataContainer
{
	GENERATED_BODY()

public:
	UEmberEquipmentManager();
	virtual int32 GetSlotCount_Implementation() const override;

	bool bIsEquipmentTag(const FGameplayTag& InTag) const;
	int32 GetSlotIndex(const FGameplayTag& InTag) const;

	virtual void MoveItemByWidget_Implementation(const FGameplayTag& SlotTag, int32 IndexTo, const TScriptInterface<UEmberSlotDataProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity) override;
	virtual int32 RemoveItemFromSlot_Implementation(int32 SlotIndex, int32 QuantityToRemove = 0) override;

	virtual int32 AddDataInIndex(const FInstancedStruct& InItem, int32 InSlotIndex) override;
protected:
	
	virtual int32 TryAddItems(const FInstancedStruct& InItem, int32 InSlotIndex = -1) override;

	virtual void InitializeInventorySlots() override;

	void RemoveEffect(int32 InSlotIndex);
	void ActiveEffect(int32 InSlotIndex);
	

protected:
	FGameplayTagContainer SlotGameplayTags;
};
