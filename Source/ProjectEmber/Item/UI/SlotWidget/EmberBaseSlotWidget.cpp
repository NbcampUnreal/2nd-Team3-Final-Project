// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseSlotWidget.h"

#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemSystemLibrary.h"
#include "Item/UI/DragDropOperation/EmberItemSlotDragDropOperation.h"

FWidgetSlotData::FWidgetSlotData(const FInventorySlotData& InSlotData)
{
	ItemID = InSlotData.ItemID;
	Quantity = InSlotData.Quantity;
	MaxStackSize = InSlotData.MaxStackSize;
	ItemDisplayName = InSlotData.ItemDisplayName;
	ItemDescription = InSlotData.ItemDescription;
	if (InSlotData.SlotData)
	{
		if (const FSlotInfoRow* SlotInfoRow = InSlotData.SlotData->GetRow<FSlotInfoRow>("SlotInfo"))
		{
			ItemIcon = SlotInfoRow->ItemIcon;
		}
	}
}

void UEmberBaseSlotWidget::InitSlot(int32 InSlotIndex, TScriptInterface<IEmberSlotDataProviderInterface> InDataProvider)
{
	SlotIndex = InSlotIndex;
	DataProvider = InDataProvider;
}

void UEmberBaseSlotWidget::SetSlotData(const FInventorySlotData& InSlotData)
{
	EMBER_LOG(LogTemp, Warning, TEXT("Slot: %s"), *InSlotData.ItemID.ToString());
	SlotData = FWidgetSlotData(InSlotData);
	EMBER_LOG(LogTemp, Warning, TEXT("Slot: %s"), *SlotData.ItemID.ToString());

	UpdateSlot();
}

void UEmberBaseSlotWidget::UpdateSlot()
{
	OnSlotDataChange();
}

void UEmberBaseSlotWidget::UpdateUI()
{
}


void UEmberBaseSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	/*if (OutOperation != nullptr)
	{
		if (TObjectPtr<UDragDropOperation> DragDropOperation = NewObject<UDragDropOperation>())
		{
			OutOperation = DragDropOperation;
		}
	}*/
}

/*
FReply UEmberBaseSlotWidget::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = Super::NativeOnMouseButtonDown(Geometry, MouseEvent);

	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (SlotData.ItemID.IsValid())
		{
			Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton);
		}
	}
	return Reply.NativeReply;

}
*/
