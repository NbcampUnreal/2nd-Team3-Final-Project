// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Condition/IInteractionConditionHandler.h"
#include "Interactables/BaseInteractableActor.h"
#include "WorldInteractableActor.generated.h"


class UInteractionCondition;
class UInteractionReceiverComponent;
class UQuestCondition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionStarted, AActor*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnded, AActor*, Interactor, bool, bCanBeTriggeredAgain);

UCLASS()
class PROJECTEMBER_API AWorldInteractableActor : public ABaseInteractableActor, public IInteractionConditionHandler
{
	GENERATED_BODY()
public:
	AWorldInteractableActor();

	virtual void HandleInteractionEvent_Implementation(const FGameplayTag& EventTag, const FGameplayEventData& EventData) override;
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void TryInteract(AActor* Interactor);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	bool CanInteract();

	UFUNCTION(BlueprintCallable, Category="Interaction")
	bool IsDeactivate();
	
	UFUNCTION()
	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION()
	virtual void EndInteract_Implementation() override;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteractionStarted OnInteractionStarted;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteractionEnded OnInteractionEnded;
	
protected:
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interaction")
	TArray<TObjectPtr<UInteractionCondition>> InteractConditions;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interaction")
	TArray<TObjectPtr<UInteractionCondition>> DeactivateConditions;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TObjectPtr<UInteractionReceiverComponent> ReceiverComponent;
};
