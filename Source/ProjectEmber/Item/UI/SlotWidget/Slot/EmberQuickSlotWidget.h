// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseDragAbleSlotWidget.h"
#include "EmberQuickSlotWidget.generated.h"

class UEmberItemSlotDragDropOperation;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberQuickSlotWidget : public UEmberBaseDragAbleSlotWidget
{
	GENERATED_BODY()
public:
	virtual FEventReply StartDragDrop_Implementation(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	UFUNCTION(BlueprintCallable)
	void UseQuickSlotItem(int32 InIndex) const;

	UFUNCTION(BlueprintCallable)
	void AddQuickSlotItem() const;
	
	UFUNCTION(BlueprintCallable)
	void SetSlotOperation(UEmberItemSlotDragDropOperation* InSlotOperation);

	virtual void UpdateSlot() override;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_UpdateUI();

	UFUNCTION()
	void UpdateQuickSlotItem(int32 InIndex, const FInstancedStruct& InSlotData);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEmberItemSlotDragDropOperation> StartSlotDropOperation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsHovered{false};
};

