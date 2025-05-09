#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Interactables/BaseInteractableActor.h"
#include "InteractableMover.generated.h"

UCLASS()
class PROJECTEMBER_API AInteractableMover : public ABaseInteractableActor
{
	GENERATED_BODY()

public:
	AInteractableMover();
	
	virtual void Interact_Implementation(AActor* Caller) override;

protected:
	
	UFUNCTION()
	void TimelineProgress(float Value);
	
private:
	UPROPERTY(EditInstanceOnly, Category="Interaction")
	AActor* TargetActor;

	FTimeline MoveTimeline;
	
	UPROPERTY()
	UCurveFloat* MoveCurve;

	FVector StartLocation;
	FVector EndLocation;
};
