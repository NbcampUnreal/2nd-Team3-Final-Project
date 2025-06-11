// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Item/Core/EmberItemStruct.h"
#include "Item/Core/EmberWidgetSlotData.h"
#include "Item/UI/SlotWidget/EmberSlotDataProviderInterface.h"
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
	void InitSlot(int32 InSlotIndex, TScriptInterface<IEmberSlotDataProviderInterface> InDataProvider);
	
	UFUNCTION(BlueprintCallable)
	void InitDetailWidget();
	
	UFUNCTION(BlueprintCallable)
	void SetSlotData(const FInstancedStruct& InSlotData);
	
	virtual void UpdateSlot();

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	int32 SlotIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	FEmberWidgetSlotData SlotData = FEmberWidgetSlotData();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	FGameplayTag SlotType;
	
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
	TScriptInterface<IEmberSlotDataProviderInterface> DataProvider = nullptr;
};
