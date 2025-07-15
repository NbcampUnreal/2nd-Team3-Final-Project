// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Fragments/InteractionFragment_Spawn_Points.h"

UInteractionFragment_Spawn_Points::UInteractionFragment_Spawn_Points()
{
}

void UInteractionFragment_Spawn_Points::OnRegister()
{
	Super::OnRegister();
}

void UInteractionFragment_Spawn_Points::SpawnActors(UWorld* World)
{
	Super::SpawnActors(World);
}

void UInteractionFragment_Spawn_Points::DestroySpawnedActors()
{
	Super::DestroySpawnedActors();
}
