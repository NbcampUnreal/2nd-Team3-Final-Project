// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemContainer/EmberBaseSlotContainer.h"
#include "EmberAddItemMessage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberAddItemMessage : public UEmberBaseSlotContainer
{
	GENERATED_BODY()
public:
	UEmberAddItemMessage();

	void StartMessage();
	virtual int32 AddSlotItemReturnApplied(const FInstancedStruct& InInstancedStruct, int32 InSlotIndex) override;
	virtual int32 AddSlotItemReturnApplied(const FName& InItemID, const int32 InQuantity, int32 InSlotIndex, const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>()) override;
	virtual int32 AddSlotItemReturnApplied(const FEmberItemEntry& InItemEntry, int32 InSlotIndex) override;

	virtual int32 MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity) override;
	
	int32 FindAddSlotIndex(const FEmberItemEntry& InItemEntry);
	virtual void CreateItemSlot(const FEmberItemEntry& InItemEntry, int32 InItemIndex) override;

	void UpdateItem();

	int32 RemoveOldMessage();
	
protected:
	FTimerHandle TimerHandle;

	float MessageLifeTime = 5.f;
};
