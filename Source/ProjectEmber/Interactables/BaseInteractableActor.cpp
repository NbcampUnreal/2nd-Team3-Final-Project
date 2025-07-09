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
	Execute_Interact(this, Caller);
}

void ABaseInteractableActor::Interact_Implementation(AActor* Caller)
{
	
}

