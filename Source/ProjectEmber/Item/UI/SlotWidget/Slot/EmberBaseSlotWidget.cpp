// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseSlotWidget.h"

#include "Item/UI/SlotWidget/DragSlotImage.h"
#include "EmberSlotDragAbleSlotInterface.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemSystemLibrary.h"
#include "Item/UI/DragDropOperation/EmberItemSlotDragDropOperation.h"

FWidgetSlotData::FWidgetSlotData(const FInstancedStruct& InSlotData)
{
	if (const FEmberSlotData* Slot = InSlotData.GetPtr<FEmberSlotData>())
	{
		UE_LOG(LogTemp, Display, TEXT("InventoryManagerComponent4: SlotData %d"),Slot->Quantity);
		ItemID = Slot->ItemID;
		Quantity = Slot->Quantity;
		MaxStackSize = Slot->MaxStackSize;
		ItemDisplayName = Slot->ItemDisplayName;
		ItemDescription = Slot->ItemDescription;
		if (Slot->SlotData)
		{
			if (const FSlotInfoRow* SlotInfoRow = Slot->SlotData->GetRow<FSlotInfoRow>("SlotInfo"))
			{
				ItemIcon = SlotInfoRow->ItemIcon;
			}
		}
	}
}

void UEmberBaseSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (SlotImage)
	{
		if (TObjectPtr<UTexture2D> SlotDefaultTexture2D = Cast<UTexture2D> (SlotImage->GetBrush().GetResourceObject()))
		{
			
			DefaultSlotTexture = SlotDefaultTexture2D;
		}
	}
}

void UEmberBaseSlotWidget::InitSlot(int32 InSlotIndex, TScriptInterface<IEmberSlotDataProviderInterface> InDataProvider)
{
	SlotIndex = InSlotIndex;
	DataProvider = InDataProvider;
	if (DataProvider)
	{
		SlotType = IEmberSlotDataProviderInterface::Execute_GetSlotType(DataProvider.GetObject());
	}
}

void UEmberBaseSlotWidget::SetSlotData(const FInstancedStruct& InSlotData)
{
	SlotData = FWidgetSlotData(InSlotData);

	UpdateSlot();
}

void UEmberBaseSlotWidget::UpdateSlot()
{
	TObjectPtr<UTexture2D> LoadTexture = nullptr;
	if (SlotData.ItemID.IsNone())
	{
		LoadTexture = DefaultSlotTexture.LoadSynchronous();
	}
	else
	{
		LoadTexture = SlotData.ItemIcon.LoadSynchronous();
	}
	SlotImage->SetBrushFromTexture(LoadTexture);

	if (SlotData.Quantity > 1)
	{
		ItemTextBlock->SetText(FText::AsNumber(SlotData.Quantity));
	}
	else
	{
		ItemTextBlock->SetText(FText::FromString(""));
	}
}

void UEmberBaseSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	IEmberSlotDragAbleSlotInterface::Execute_CreateDragDropOperation(this, InGeometry, InMouseEvent, OutOperation);
}

FReply UEmberBaseSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	Reply = IEmberSlotDragAbleSlotInterface::Execute_StartDragDrop(this, InGeometry, InMouseEvent).NativeReply;
	return Reply;
}

bool UEmberBaseSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return IEmberSlotDragAbleSlotInterface::Execute_DropAction(this, InGeometry, InDragDropEvent, InOperation) ||
		Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
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
