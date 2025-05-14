#pragma once

#include "CoreMinimal.h"
#include "Gimic.h"
#include "Interactables/BaseInteractableActor.h"
#include "GimicTriggerActor.generated.h"

UCLASS()
class PROJECTEMBER_API AGimicTriggerActor : public ABaseInteractableActor
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void Interact_Implementation(AActor* Caller) override;
	
	UFUNCTION()
	void SetCurrentInteractable(UObject* NewGimic);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gimic Interface")
	TScriptInterface<IGimic> CurrentGimic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gimic Actor")
	AActor* GimicActor;
};
