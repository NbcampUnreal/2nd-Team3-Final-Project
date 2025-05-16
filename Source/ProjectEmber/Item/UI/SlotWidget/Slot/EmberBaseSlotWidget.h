// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Item/Core/EmberItemStruct.h"
#include "Item/UI/SlotWidget/EmberSlotDataProviderInterface.h"
#include "EmberBaseSlotWidget.generated.h"

class UItemDetailWidget;
class UDragSlotImage;
class UImage;

/**
 * 
 */

USTRUCT(BlueprintType)
struct FWidgetSlotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot")
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot", meta = (ClampMin = "0"))
	int32 Quantity = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	int32 MaxStackSize = 1;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
	FText ItemDisplayName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
	FText ItemDescription = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
	TSoftObjectPtr<UTexture2D> ItemIcon = nullptr;
    
	FWidgetSlotData() = default;

	FWidgetSlotData(const FInventorySlotData& InSlotData);
    
	bool IsEmpty() const { return ItemID.IsNone() || Quantity <= 0; }
    
	void Clear()
	{
		ItemID = NAME_None;
		Quantity = 0;
		MaxStackSize = 1;
		ItemDisplayName = FText();
		ItemDescription = FText();
		ItemIcon = nullptr;
	}
};

UCLASS()
class PROJECTEMBER_API UEmberBaseSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	UFUNCTION(BlueprintCallable)
	void InitSlot(int32 InSlotIndex, TScriptInterface<IEmberSlotDataProviderInterface> InDataProvider);
	
	UFUNCTION(BlueprintCallable)
	void SetSlotData(const FInventorySlotData& InSlotData);
	
	virtual void UpdateSlot();

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override; 
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	int32 SlotIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	FWidgetSlotData SlotData = FWidgetSlotData();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	FGameplayTag SlotType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> SlotImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemTextBlock;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	TSoftObjectPtr<UTexture2D> DefaultSlotTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	TSubclassOf<UItemDetailWidget> ItemDetailWidget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<IEmberSlotDataProviderInterface> DataProvider = nullptr;
};
