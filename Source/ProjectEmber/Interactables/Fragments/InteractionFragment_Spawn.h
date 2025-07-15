// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Fragments/InteractionFragment.h"
#include "InteractionFragment_Spawn.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class PROJECTEMBER_API UInteractionFragment_Spawn : public UInteractionFragment
{
	GENERATED_BODY()
public:
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;
	virtual void EndInteraction_Implementation() override;
	
	virtual void SpawnActors(UWorld* WorldContext) {}

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	virtual void DestroySpawnedActors() {}
};