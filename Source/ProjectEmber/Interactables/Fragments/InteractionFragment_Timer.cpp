// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Fragments/InteractionFragment_Timer.h"

void UInteractionFragment_Timer::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);
	
	StopTimer(); // 기존 타이머 제거 (중복 방지)
	
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UInteractionFragment_Timer, OnTimerExpired), Interactor);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, Duration, false);
}

void UInteractionFragment_Timer::EndInteraction_Implementation()
{
	Super::EndInteraction_Implementation();
	StopTimer();
}

void UInteractionFragment_Timer::StopTimer()
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		if (TimerManager.IsTimerActive(TimerHandle))
		{
			TimerManager.ClearTimer(TimerHandle);
		}
	}
}

void UInteractionFragment_Timer::OnTimerExpired(AActor* Interactor)
{
	if (!Interactor) return;

	OnTimerExpiredEvent.Broadcast(Interactor);
}
