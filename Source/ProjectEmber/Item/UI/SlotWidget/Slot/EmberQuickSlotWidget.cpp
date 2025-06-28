// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberQuickSlotWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character/EmberCharacter.h"
#include "Components/Image.h"
#include "EmberLog/EmberLog.h"
#include "Item/QuickSlotManager.h"
#include "Item/UserItemManger.h"
#include "Item/ItemContainer/Implements/EmberBaseResourceSlotContainer/EmberQuickSlotContainer.h"
#include "Item/UI/DragDropOperation/EmberItemSlotDragDropOperation.h"
#include "Kismet/GameplayStatics.h"

FEventReply UEmberQuickSlotWidget::StartDragDrop_Implementation(const FGeometry& InGeometry,
                                                                const FPointerEvent& InMouseEvent)
{
	FEventReply Reply = Super::StartDragDrop_Implementation(InGeometry, InMouseEvent);
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && DataProvider && StartSlotDropOperation)
	{
		AddQuickSlotItem();
		return UWidgetBlueprintLibrary::Handled();
		// 클릭 즉시 drag발동
		/*// 클릭하면서 움직임이있을경우 발동
		FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, InMouseEvent.GetEffectingButton());
		return Reply.NativeReply;*/
	}
	return UWidgetBlueprintLibrary::Unhandled();
}


void UEmberQuickSlotWidget::UseQuickSlotItem(int32 InIndex) const
{

	if (DataProvider && IsValid(DataProvider.GetObject()))
	{
		IEmberSlotProviderInterface::Execute_UseItemInSlot(DataProvider.GetObject(), InIndex);
	}
}

void UEmberQuickSlotWidget::AddQuickSlotItem() const
{
	if (DataProvider && IsValid(DataProvider.GetObject()))
	{
		IEmberSlotProviderInterface::Execute_MoveItemByWidget(DataProvider.GetObject(), StartSlotDropOperation->SlotType, SlotIndex, StartSlotDropOperation->Provider.GetObject(), StartSlotDropOperation->SlotIndex, StartSlotDropOperation->DraggedQuantity);
	}
}

void UEmberQuickSlotWidget::SetSlotOperation(UEmberItemSlotDragDropOperation* InSlotOperation)
{
	StartSlotDropOperation = InSlotOperation;

	if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>( UGameplayStatics::GetPlayerCharacter(GetWorld(),0)))
	{
		EmberCharacter->GetItemManager()->QuickSlotManager->OnItemChangedDelegate.AddDynamic(this, &UEmberQuickSlotWidget::UpdateQuickSlotItem);
	}
}

void UEmberQuickSlotWidget::UpdateSlot()
{
	Super::UpdateSlot();
	BP_UpdateUI();
}

void UEmberQuickSlotWidget::UpdateQuickSlotItem(int32 InIndex, const FInstancedStruct& InSlotData)
{

	if (SlotIndex == InIndex)
	{
		SetSlotData(InSlotData);
	}
}
