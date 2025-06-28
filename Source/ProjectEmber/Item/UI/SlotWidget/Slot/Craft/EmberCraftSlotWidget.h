// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/UI/DragDropOperation/EmberItemSlotDragDropOperation.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseDragAbleSlotWidget.h"
#include "EmberCraftSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberCraftSlotWidget : public UEmberBaseDragAbleSlotWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void InitCraftComponent(TScriptInterface<IEmberSlotProviderInterface> InDataProvider, int32 InSlotIndex);

	void UnBindToManagerDelegates();
	UFUNCTION()
	void SlotChanged(int32 InIndex, const FInstancedStruct& InSlotData);
	
	virtual void UpdateSlot() override;
};
