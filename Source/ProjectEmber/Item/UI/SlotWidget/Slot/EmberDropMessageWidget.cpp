// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropMessageWidget.h"

void UEmberDropMessageWidget::StartShowTime()
{
	if (bCanVisible())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UEmberDropMessageWidget::EndShowTime, 5.f, false);
		this->SetVisibility(ESlateVisibility::Visible);
	}
}

void UEmberDropMessageWidget::EndShowTime()
{
	this->SetVisibility(ESlateVisibility::Hidden);

}

bool UEmberDropMessageWidget::bCanVisible()
{
	if (Super::bCanVisible() && SlotData.Quantity > 0)
	{
		return true;
	}
	return false;
}
