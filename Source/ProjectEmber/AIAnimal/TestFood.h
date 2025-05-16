// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Interactables/BaseInteractableActor.h"
#include "TestFood.generated.h"

UCLASS()
class PROJECTEMBER_API ATestFood : public ABaseInteractableActor
{
	GENERATED_BODY()

public:
	ATestFood();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Interact_Implementation(AActor* Caller) override;
	virtual float GetGatherTime_Implementation() override; 

	
	UFUNCTION(BlueprintCallable)
	bool GetSelected();
	
	UFUNCTION(BlueprintCallable)
	const FGameplayTagContainer& GetGameplayTagContainer();

	UFUNCTION(BlueprintCallable)
	void SetSelected(const bool InIsSelected);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	bool bIsSelected = false;
};
