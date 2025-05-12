#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractableActor.h"
#include "MiningActor.generated.h"

UCLASS()
class PROJECTEMBER_API AMiningActor : public ABaseInteractableActor
{
	GENERATED_BODY()

public:
	virtual void Interact_Implementation(AActor* Caller) override;
	virtual float GetGatherTime_Implementation() override;	

private:
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Actor")
	TSubclassOf<ABaseInteractableActor> SpawnActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "GatherTime")
	float GatherTime = 0.0f;
};
