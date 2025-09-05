// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Fragments/InteractionFragment.h"

void UInteractionFragment::TryExecuteInteraction(AActor* Interactor)
{
	if (!CanExecuteInteraction(Interactor)) return;

	ExecuteInteraction(Interactor);
}

bool UInteractionFragment::CanExecuteInteraction_Implementation(AActor* Interactor) const
{
	return bInteractionEnabled;
}

void UInteractionFragment::ExecuteInteraction_Implementation(AActor* Interactor)
{
}

void UInteractionFragment::EndInteraction_Implementation()
{
}