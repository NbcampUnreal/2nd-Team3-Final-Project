// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/UI/SlotWidget/SlotsPanel/EmberBasePanel.h"
#include "EmberQuickSlotPanel.generated.h"

class UEmberItemSlotDragDropOperation;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberQuickSlotPanel : public UEmberBasePanel
{
	GENERATED_BODY()
public:
	virtual void BP_SetProvider_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void SetSlotOperation(UEmberItemSlotDragDropOperation* InSlotOperation);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEmberItemSlotDragDropOperation> SlotOperation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FocusIndex;
};
