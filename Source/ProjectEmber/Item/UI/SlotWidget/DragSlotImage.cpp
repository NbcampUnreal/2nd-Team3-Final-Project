// Fill out your copyright notice in the Description page of Project Settings.


#include "DragSlotImage.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "EmberLog/EmberLog.h"

void UDragSlotImage::InitImage(const UImage* InImage) const
{
	if (InImage)
	{
		SlotImage->SetBrush(InImage->GetBrush());

		FDeprecateSlateVector2D SlotImageSize = InImage->GetCachedGeometry().ToPaintGeometry().GetLocalSize();
		float Scale = InImage->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
		FVector2f FinalSize = SlotImageSize * Scale;
		SlotSizeBox->SetWidthOverride(FinalSize.X);
		SlotSizeBox->SetHeightOverride(FinalSize.Y);

	}
}

