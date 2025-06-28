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
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	UAnimMontage* HitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Block")
	bool bIsBlockHit{false};
	
	UFUNCTION()
	void OnMontageFinished();
};
