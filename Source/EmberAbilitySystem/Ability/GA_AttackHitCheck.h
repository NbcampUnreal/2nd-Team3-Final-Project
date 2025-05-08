// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API UGA_AttackHitCheck : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_AttackHitCheck();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSubclassOf<class UGameplayEffect> AttackDamageEffectClass;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSubclassOf<class UGameplayEffect> AttackBuffEffectClass;
	
	float CurrentLevel;
};
