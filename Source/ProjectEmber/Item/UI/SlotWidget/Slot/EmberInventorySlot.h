// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseDragAbleSlotWidget.h"
#include "EmberInventorySlot.generated.h"

class UEmberQuickSlotsPanel;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberInventorySlot : public UEmberBaseDragAbleSlotWidget
{
	GENERATED_BODY()

	virtual FEventReply StartDragDrop_Implementation(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEmberQuickSlotsPanel> QuickSlotWidget;
};
