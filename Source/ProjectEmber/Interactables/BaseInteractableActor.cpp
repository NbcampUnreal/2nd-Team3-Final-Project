#include "Interactables/BaseInteractableActor.h"

ABaseInteractableActor::ABaseInteractableActor()
{
 	PrimaryActorTick.bCanEverTick = false;

}

void ABaseInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseInteractableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABaseInteractableActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ABaseInteractableActor::Interact_Implementation(AActor* Caller)
{
	IInteractable::Interact_Implementation(Caller);
}

