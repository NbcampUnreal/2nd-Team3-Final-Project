// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseDragAbleSlotWidget.h"
#include "EmberInventorySlotWidget.generated.h"

class UEmberQuickSlotPanel;
class UEmberQuickSlotsPanel;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberInventorySlotWidget : public UEmberBaseDragAbleSlotWidget
{
	GENERATED_BODY()

	virtual FEventReply StartDragDrop_Implementation(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEmberQuickSlotPanel> QuickSlotWidget;
};
