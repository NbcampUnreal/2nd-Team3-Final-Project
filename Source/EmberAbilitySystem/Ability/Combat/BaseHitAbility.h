// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseHitAbility.generated.h"

/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API UBaseHitAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBaseHitAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "ParryCounter")
	UAnimMontage* HitMontage;
	
	UFUNCTION()
	void OnMontageFinished();
};
