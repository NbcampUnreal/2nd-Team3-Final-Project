// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Fragments/InteractionFragment.h"
#include "InteractionFragment_RaidStart.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInteractionFragment_RaidStart : public UInteractionFragment
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;
	virtual void EndInteraction_Implementation() override;
	
	void OnStartRaid();
};