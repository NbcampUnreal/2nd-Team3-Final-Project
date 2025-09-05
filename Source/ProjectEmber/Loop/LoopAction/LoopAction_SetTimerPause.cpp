// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/LoopAction/LoopAction_SetTimerPause.h"

#include "Fragment/LoopActionFragment_Bool.h"
#include "Loop/LoopEventDirector.h"
#include "Loop/LoopTimeOfDaySubsystem.h"

void ULoopAction_SetTimerPause::ActivateAction_Implementation()
{
	Super::ActivateAction_Implementation();

	auto* BoolFragment = FindFragment<ULoopActionFragment_Bool>();

	ULoopTimeOfDaySubsystem* Subsystem = OwnerDirector->GetWorld()->GetSubsystem<ULoopTimeOfDaySubsystem>();
	
	if (BoolFragment)
	{
		Subsystem->SetPause(BoolFragment->Value);
	}

	FinishAction();
}
