// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/UI/SlotWidget/SlotsPanel/EmberBasePanel.h"
#include "EmberEquipmentPanel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberEquipmentPanel : public UEmberBasePanel
{
	GENERATED_BODY()
public:
	virtual void BP_SetProvider_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Slots Panel")
	int32 GetSlotIndex(const FGameplayTag& InTag) const;
};
