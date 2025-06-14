// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/InteractionReceiverComponent.h"

UInteractionReceiverComponent::UInteractionReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInteractionReceiverComponent::BroadCastInteractionCompleted(AActor* CompletedBy)
{
	OnInteractionCompleted.Broadcast(CompletedBy, bCanBeTriggeredAgain);
}

