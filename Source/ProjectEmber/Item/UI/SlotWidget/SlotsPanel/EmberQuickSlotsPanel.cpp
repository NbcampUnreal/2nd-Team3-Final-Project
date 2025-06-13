// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberQuickSlotsPanel.h"

#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "Item/UserItemManger.h"
#include "Item/QuickSlotManager.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"
#include "Item/UI/SlotWidget/Slot/EmberQuickSlotWidget.h"

void UEmberQuickSlotsPanel::BP_SetProvider_Implementation()
{
	Super::BP_SetProvider_Implementation();
	
	if (GetOwningPlayer() && GetOwningPlayer()->GetCharacter())
	{
		if (TObjectPtr<UUserItemManger> ItemManger = GetOwningPlayer()->GetCharacter()->GetComponentByClass<UUserItemManger>())
		{
			DataProvider = ItemManger->GetQuickSlotManager();
			if (!DataProvider)
			{
				EMBER_LOG(LogEmberItem, Warning, TEXT("DataProvider IsNull"));
			}
		}
	}
}

void UEmberQuickSlotsPanel::SetSlotOperation(UEmberItemSlotDragDropOperation* InSlotOperation)
{
	SlotOperation = InSlotOperation;
	for (auto& DataSlot : Slots)
	{
		if (UEmberQuickSlotWidget* Widget = Cast<UEmberQuickSlotWidget>(DataSlot))
		{
			Widget->SetSlotOperation(SlotOperation);
		}
	}
}
FReply UEmberQuickSlotsPanel::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::MiddleMouseButton))
	{
		this->SetVisibility(ESlateVisibility::Hidden);
	}

	return Reply;
}



