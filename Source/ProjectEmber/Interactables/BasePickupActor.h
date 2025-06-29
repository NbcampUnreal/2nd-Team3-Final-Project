#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractableActor.h"
#include "BasePickupActor.generated.h"

UCLASS()
class PROJECTEMBER_API ABasePickupActor : public ABaseInteractableActor
{
	GENERATED_BODY()

public:
	ABasePickupActor();
	virtual void TryInteract_Implementation(AActor* Caller) override;
	virtual void Interact_Implementation(AActor* Caller) override;
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;
	
};
