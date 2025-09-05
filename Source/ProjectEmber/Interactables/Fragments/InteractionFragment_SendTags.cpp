// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Fragments/InteractionFragment_SendTags.h"

#include "GameplayTagContainer.h"
#include "GameInstance/GameplayEventSubsystem.h"
#include "Interactables/WorldInteractableActor.h"

void UInteractionFragment_SendTags::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);

	if (!TargetActor.IsValid()) return;
	
	for (FGameplayTag& tag : StartTags)
	{
		TargetActor->HandleEventTag(tag, FGameplayEventData());
	}
}

void UInteractionFragment_SendTags::EndInteraction_Implementation()
{
	Super::EndInteraction_Implementation();

	if (!TargetActor.IsValid()) return;
	
	for (FGameplayTag& tag : EndTags)
	{
		TargetActor->HandleEventTag(tag, FGameplayEventData());
	}
}
