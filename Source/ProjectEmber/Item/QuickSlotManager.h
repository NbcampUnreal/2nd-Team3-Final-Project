// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManager.h"
#include "Components/ActorComponent.h"
#include "QuickSlotManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuickSlotChangedDelegate, int32, SlotIndex,	const FEmberSlotData&, SlotData);

UCLASS(ClassGroup=(Custom))
class PROJECTEMBER_API UQuickSlotManager : public UEmberDataContainer
{
	GENERATED_BODY()
	
public:
	FName SelectQuickSlot(int32 InIndex);

	UPROPERTY(BlueprintAssignable, Category="QuickSlot")
	FOnQuickSlotChangedDelegate OnQuickSlotChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="QuickSlot")
	int32 CurrentQuickSlotIndex = -1;
};
