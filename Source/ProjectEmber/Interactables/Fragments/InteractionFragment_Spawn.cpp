// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Fragments/InteractionFragment_Spawn.h"

void UInteractionFragment_Spawn::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);
	UWorld* World = GetOwner()->GetWorld();
	SpawnActors(World);
}

void UInteractionFragment_Spawn::EndInteraction_Implementation()
{
	Super::EndInteraction_Implementation();
	DestroySpawnedActors();
}
