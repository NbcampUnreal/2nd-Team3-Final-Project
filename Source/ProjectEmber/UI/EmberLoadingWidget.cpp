// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberLoadingWidget.h"
#include "Components/ProgressBar.h"

void UEmberLoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Progress = 0.0f;

	if (LoadingProgressBar)
	{
		LoadingProgressBar->SetPercent(0.0f);
	}
}

void UEmberLoadingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Progress = FMath::Clamp(Progress + InDeltaTime * 0.1f, 0.0f, 1.0f);

	if (LoadingProgressBar)
	{
		LoadingProgressBar->SetPercent(Progress);
	}
}