// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/WorldInteractableActor.h"

#include "InteractionFragment.h"
#include "InteractionReceiverComponent.h"


AWorldInteractableActor::AWorldInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	ReceiverComponent = CreateDefaultSubobject<UInteractionReceiverComponent>(TEXT("ReceiverComponent"));
	ReceiverComponent->ActivateAction1.AddDynamic(this, &AWorldInteractableActor::Interact);
}

void AWorldInteractableActor::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);

	TArray<UInteractionFragment*> Fragments;
	GetComponents<UInteractionFragment>(Fragments);

	for (UInteractionFragment* Fragment : Fragments)
	{
		if (Fragment)
		{
			Fragment->ExecuteInteraction(Interactor);
		}
	}
}
