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

void ABaseInteractableActor::Interact_Implementation(AActor* Caller)
{
	IInteractable::Interact_Implementation(Caller);
}

float ABaseInteractableActor::GetGatherTime_Implementation()
{
	return 0.0f;
}

