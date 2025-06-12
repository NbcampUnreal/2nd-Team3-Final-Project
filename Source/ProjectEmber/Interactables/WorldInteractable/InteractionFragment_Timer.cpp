// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/InteractionFragment_Timer.h"

#include "WorldInteractableActor.h"

void UInteractionFragment_Timer::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);

	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, "OnTimerExpired", Interactor);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, Duration, false);
}

void UInteractionFragment_Timer::EndInteraction_Implementation()
{
	Super::EndInteraction_Implementation();
	StopTimer();
}

void UInteractionFragment_Timer::StopTimer()
{
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
}

void UInteractionFragment_Timer::OnTimerExpired(AActor* Interactor)
{
	if (!Interactor) return;
	AActor* Owner = GetOwner();
	if (Owner && Owner->Implements<UInteractable>())
	{
		IInteractable::Execute_EndInteract(Owner);
	}
}
