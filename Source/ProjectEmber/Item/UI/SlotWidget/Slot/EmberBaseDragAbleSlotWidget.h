// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseSlotWidget.h"
#include "EmberSlotDragAbleSlotInterface.h"
#include "EmberBaseDragAbleSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberBaseDragAbleSlotWidget : public UEmberBaseSlotWidget, public IEmberSlotDragAbleSlotInterface
{
	GENERATED_BODY()

public:
	virtual void CreateDragDropOperation_Implementation(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation) override;
	virtual FEventReply StartDragDrop_Implementation(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual bool DropAction_Implementation(const FGeometry& InGeometry, const FPointerEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	TSubclassOf<UDragSlotImage> DragSlotImageClass = nullptr;

	
};
