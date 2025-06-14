// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ParryCounterAbility.generated.h"

/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API UParryCounterAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UParryCounterAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "ParryCounter")
	UAnimMontage* ParryCounterMontage;
	
	UFUNCTION()
	void OnMontageFinished();
};
