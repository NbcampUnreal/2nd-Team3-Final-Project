#include "Interactables/BaseInteractableActor.h"
#include "Character/EmberComponents/InteractionComponent.h"

ABaseInteractableActor::ABaseInteractableActor()
{
 	PrimaryActorTick.bCanEverTick = false;
	
}

void ABaseInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseInteractableActor::TryInteract_Implementation(AActor* Caller)
{
	Interact(Caller);
}

void ABaseInteractableActor::Interact_Implementation(AActor* Caller)
{
	
}

float ABaseInteractableActor::GetGatherTime_Implementation()
{
	return 0.0f;
}

