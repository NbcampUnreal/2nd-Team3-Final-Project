#include "Interactables/GimicActors/GimicTriggerActor.h"

#include "EmberLog/EmberLog.h"

void AGimicTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	if (GimicActor)
	{
		if (GimicActor->Implements<UGimic>())
		{
			SetCurrentInteractable(GimicActor);
		}
		else
		{
			EMBER_LOG(LogTemp, Warning, TEXT("IGimic is Not Imlements"));
		}
	}
	else
	{
		EMBER_LOG(LogTemp, Warning, TEXT("Gimic is NULL"));
	}
}

void AGimicTriggerActor::Interact_Implementation(AActor* Caller)
{
	IGimic::Execute_PlayGimic(CurrentGimic.GetObject());
}

void AGimicTriggerActor::SetCurrentInteractable(UObject* NewGimic)
{
	CurrentGimic = NewGimic;
}
