// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoopAndTimeUserWidget.h"

#include "Components/TextBlock.h"
#include "Loop/GameLoopManagerSubsystem.h"
#include "Loop/LoopTimeOfDaySubsystem.h"

void ULoopAndTimeUserWidget::Update()
{
	ULoopTimeOfDaySubsystem* TOD = GetWorld()->GetSubsystem<ULoopTimeOfDaySubsystem>();
	
	if (!TOD) return;
	
	if (Hour_Text)
	{
		Hour_Text->SetText(FText::AsNumber(TOD->GetHour()));
	}

	UGameLoopManagerSubsystem* LoopManager = GetWorld()->GetGameInstance()->GetSubsystem<UGameLoopManagerSubsystem>();

	if (!LoopManager) return;

	if (CurrentLoop_Text)
	{
		CurrentLoop_Text->SetText(FText::AsNumber(LoopManager->GetCurrentLoopID()));
	}
}

void ULoopAndTimeUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Update();
}
