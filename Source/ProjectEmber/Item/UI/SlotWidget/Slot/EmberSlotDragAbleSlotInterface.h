// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Input/DragAndDrop.h"
#include "Layout/Geometry.h"           
#include "Input/Events.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/SlateWrapperTypes.h"
#include "UObject/Interface.h" 
#include "EmberSlotDragAbleSlotInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UEmberSlotDragAbleSlotInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTEMBER_API IEmberSlotDragAbleSlotInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CreateDragDropOperation(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FEventReply StartDragDrop(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool DropAction(const FGeometry& InGeometry, const FPointerEvent& InDragDropEvent,
	UDragDropOperation* InOperation);
};
