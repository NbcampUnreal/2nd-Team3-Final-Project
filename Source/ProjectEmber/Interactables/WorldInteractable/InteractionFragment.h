// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionFragment.generated.h"

UCLASS(Abstract, Blueprintable, ClassGroup=(Interactor), meta=(BlueprintSpawnableComponent))
class UInteractionFragment : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void ExecuteInteraction(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void EndInteraction();
};
