// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDetailWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemTypes.h"


void UItemDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (TObjectPtr<APlayerController> PlayerController = GetOwningPlayer())
	{
		FVector2D MousePosition;
		UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PlayerController, MousePosition.X, MousePosition.Y);
		this->SetPositionInViewport(MousePosition, false);
	
	}
}

void UItemDetailWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TObjectPtr<APlayerController> PlayerController = GetOwningPlayer())
	{
		FVector2D MousePosition;
		UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PlayerController, MousePosition.X, MousePosition.Y);
		this->SetPositionInViewport(MousePosition, false);
	
	}
}

FString UItemDetailWidget::GetEffectsName() const
{
	FString EffectNames = "";
	for (const FItemEffectApplicationInfo& Effect : EmberWidgetSlotData.Enchants)
	{
		EffectNames += Effect.EffectName.ToString() + '\n';
	}
	return EffectNames;
}
