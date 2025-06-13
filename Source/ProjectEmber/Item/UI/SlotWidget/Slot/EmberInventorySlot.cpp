// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberInventorySlot.h"
#include "EmberQuickSlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EmberLog/EmberLog.h"
#include "Item/ItemSubsystem.h"
#include "Item/Core/EmberItemDeveloperSetting.h"
#include "Item/Core/ItemSystemLibrary.h"
#include "Item/UI/DragDropOperation/EmberItemSlotDragDropOperation.h"
#include "Item/UI/SlotWidget/SlotsPanel/EmberQuickSlotsPanel.h"


FEventReply UEmberInventorySlot::StartDragDrop_Implementation(const FGeometry& InGeometry,
                                                              const FPointerEvent& InMouseEvent)
{
	FEventReply Reply = Super::StartDragDrop_Implementation(InGeometry, InMouseEvent);
	if (!SlotData.ItemID.IsNone() && InMouseEvent.IsMouseButtonDown(EKeys::MiddleMouseButton))
	{
		if (UItemSubsystem* ItemSubsystem = UItemSystemLibrary::GetItemSubsystem())
		{
			QuickSlotWidget = ItemSubsystem->GetQuickSlotWidget();

			if (QuickSlotWidget)
			{
				QuickSlotWidget->SetVisibility(ESlateVisibility::Visible);
				
				if (TObjectPtr<UEmberItemSlotDragDropOperation> SlotOperation = NewObject<UEmberItemSlotDragDropOperation>(GetTransientPackage(), UEmberItemSlotDragDropOperation::StaticClass()))
				{
					SlotOperation->SlotIndex = SlotIndex;
					SlotOperation->SlotType = SlotType;
					SlotOperation->DraggedQuantity = SlotData.Quantity;
					SlotOperation->Provider = DataProvider;
					QuickSlotWidget->SetSlotOperation(SlotOperation);
				}

			}
		}
	}
	
	return Reply;

}

