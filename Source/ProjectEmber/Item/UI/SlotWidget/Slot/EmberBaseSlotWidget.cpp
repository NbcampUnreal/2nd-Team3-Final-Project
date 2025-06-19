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
#include "Item/UI/DragDropOperation/EmberItemSlotDragDropOperation.h"
#include "Item/UI/SlotWidget/ItemDetailWidget.h"

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
	InitDetailWidget();
	if (DataProvider && DataProvider.GetObject())
	{
		SlotType = IEmberSlotDataProviderInterface::Execute_GetSlotType(DataProvider.GetObject());
	}
}

void UEmberBaseSlotWidget::SetSlotData(const FInstancedStruct& InSlotData)
{
	SlotData = FEmberWidgetSlotData(InSlotData);
	
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

	EMBER_LOG(LogTemp, Warning, TEXT("ABCDSE %s %d"), *SlotData.ItemID.ToString(), SlotData.Quantity);

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
			IEmberSlotDataProviderInterface::Execute_UseItemInSlot(DataProvider.GetObject(), SlotIndex);
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

	if (ItemDetailWidget && !SlotData.IsEmpty())
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

