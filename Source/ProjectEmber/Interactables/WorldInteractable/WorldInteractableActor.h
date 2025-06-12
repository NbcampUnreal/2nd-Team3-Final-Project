// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractableActor.h"
#include "WorldInteractableActor.generated.h"


class UInteractionReceiverComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionStarted, AActor*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionEnded, AActor*, Interactor);

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

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteractionStarted OnInteractionStarted;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteractionEnded OnInteractionEnded;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TObjectPtr<UInteractionReceiverComponent> ReceiverComponent;
};
