// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Fragments/InteractionFragment.h"
#include "InteractionFragment_SendTags.generated.h"

class AWorldInteractableActor;
struct FGameplayTag;
/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInteractionFragment_SendTags : public UInteractionFragment
{
	GENERATED_BODY()

public:
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;
	virtual void EndInteraction_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TWeakObjectPtr<AWorldInteractableActor> TargetActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TArray<FGameplayTag> StartTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TArray<FGameplayTag> EndTags;
};
