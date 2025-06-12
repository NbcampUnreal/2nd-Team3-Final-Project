// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractableActor.h"
#include "WorldInteractableActor.generated.h"


class UInteractionReceiverComponent;

UCLASS()
class PROJECTEMBER_API AWorldInteractableActor : public ABaseInteractableActor
{
	GENERATED_BODY()
public:
	AWorldInteractableActor();

	UFUNCTION()
	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION()
	virtual void EndInteract_Implementation() override;

protected:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInteractionReceiverComponent> ReceiverComponent;
};
