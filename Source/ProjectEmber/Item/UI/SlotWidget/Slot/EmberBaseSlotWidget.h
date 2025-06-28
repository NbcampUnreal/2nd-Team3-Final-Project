// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Core/EmberItemStruct.h"
#include "Item/Core/ItemStruct/Implements/EmberSlot/EmberWidgetSlot.h"
#include "Item/UI/EmberSlotProviderInterface.h"
#include "EmberBaseSlotWidget.generated.h"

class UItemDetailWidget;
class UDragSlotImage;
class UImage;

/**
 * 
 */

UCLASS()
class PROJECTEMBER_API UEmberBaseSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	UFUNCTION(BlueprintCallable)
	void InitSlot(int32 InSlotIndex, TScriptInterface<IEmberSlotProviderInterface> InDataProvider);
	
	UFUNCTION(BlueprintCallable)
	void InitDetailWidget();
	
	UFUNCTION(BlueprintCallable)
	virtual void SetSlotData(const FInstancedStruct& InSlotData);

	UFUNCTION(BlueprintCallable)
	virtual void UpdateSlot();

	virtual bool bCanVisible();

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	int32 SlotIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	FEmberWidgetSlot SlotData = FEmberWidgetSlot();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> SlotImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemTextBlock;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	TSoftObjectPtr<UTexture2D> DefaultSlotTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	TSubclassOf<UItemDetailWidget> ItemDetailWidgetClass = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	TObjectPtr<UItemDetailWidget> ItemDetailWidget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<IEmberSlotProviderInterface> DataProvider = nullptr;
};
