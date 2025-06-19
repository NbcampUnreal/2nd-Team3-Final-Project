// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/ItemCraftType.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"
#include "EmberResourceSlot.generated.h"

struct FEmberMasterItemData;
class UEmberCraftComponent;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberResourceSlot : public UEmberBaseSlotWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Craft")
	void InitCraft(UEmberCraftComponent* InCraftComponent, const FCraftPair& InRequestInfo);
	
	UFUNCTION(BlueprintCallable)
	float GetPercentage();

	UFUNCTION(BlueprintCallable)
	void UpdateCraftSlot();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredQuantity = 0;
	
	TObjectPtr<UEmberCraftComponent> CraftComponent = nullptr;
};
