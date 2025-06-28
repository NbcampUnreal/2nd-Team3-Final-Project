// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberBaseSlotWidget.h"

#include "Item/UI/SlotWidget/DragSlotImage.h"
#include "EmberSlotDragAbleSlotInterface.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "EmberLog/EmberLog.h"
#include "Item/ItemSubsystem.h"
#include "Item/Core/ItemSystemLibrary.h"
#include "Item/UI/EmberSlotProviderInterface.h"
#include "Item/UI/SlotWidget/ItemDetailWidget.h"

void UEmberBaseSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (SlotImage)
	{
		if (TObjectPtr<UTexture2D> SlotDefaultTexture2D = Cast<UTexture2D> (SlotImage->GetBrush().GetResourceObject()))
		{
			if (!DefaultSlotTexture)
			{
				DefaultSlotTexture = SlotDefaultTexture2D;
			}
		}
	}
}

void UEmberBaseSlotWidget::InitSlot(int32 InSlotIndex, TScriptInterface<IEmberSlotProviderInterface> InDataProvider)
{
	SlotIndex = InSlotIndex;
	DataProvider = InDataProvider;
	InitDetailWidget();
}

void UEmberBaseSlotWidget::SetSlotData(const FInstancedStruct& InSlotData)
{
	SlotData = FEmberWidgetSlot(InSlotData);
	
	UpdateSlot();
}

void UEmberBaseSlotWidget::InitDetailWidget()
{
	if (UItemSubsystem* ItemSubsystem = UItemSystemLibrary::GetItemSubsystem())
	{
		ItemDetailWidget = ItemSubsystem->GetItemDetailWidget();
	}
}

void UEmberBaseSlotWidget::UpdateSlot()
{
	TObjectPtr<UTexture2D> LoadTexture = nullptr;

	if (bCanVisible())
	{
		LoadTexture = SlotData.SlotInfo.ItemIcon.LoadSynchronous();
		SlotImage->SetColorAndOpacity((FLinearColor(1.f,1.f,1.f, 1.f)));
	}
	else
	{
		LoadTexture = DefaultSlotTexture.LoadSynchronous();
		SlotImage->SetColorAndOpacity((FLinearColor(1.f,1.f,1.f, 0.5f)));
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

bool UEmberBaseSlotWidget::bCanVisible()
{
	if (SlotData.ItemID.IsNone())
	{
		return false;
	}
	return true;
}

void UEmberBaseSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (Cast<IEmberSlotDragAbleSlotInterface>(this))
	{
		IEmberSlotDragAbleSlotInterface::Execute_CreateDragDropOperation(this, InGeometry, InMouseEvent, OutOperation);
	}
}

FReply UEmberBaseSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (Cast<IEmberSlotDragAbleSlotInterface>(this))
	{
		Reply = IEmberSlotDragAbleSlotInterface::Execute_StartDragDrop(this, InGeometry, InMouseEvent).NativeReply;

	}
	return Reply;
}

FReply UEmberBaseSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (!SlotData.ItemID.IsNone() && DataProvider && DataProvider.GetObject())
	{
		if (Cast<IEmberSlotDragAbleSlotInterface>(this))
		{
			IEmberSlotProviderInterface::Execute_UseItemInSlot(DataProvider.GetObject(), SlotIndex);
		}
		return UWidgetBlueprintLibrary::Handled().NativeReply;
	}
	return Reply;
}

bool UEmberBaseSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                        UDragDropOperation* InOperation)
{
	return IEmberSlotDragAbleSlotInterface::Execute_DropAction(this, InGeometry, InDragDropEvent, InOperation) ||
		Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UEmberBaseSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (ItemDetailWidget && !SlotData.ItemID.IsNone())
	{
		ItemDetailWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		ItemDetailWidget->EmberWidgetSlotData = SlotData;
	}
}

void UEmberBaseSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (ItemDetailWidget)
	{
		ItemDetailWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

