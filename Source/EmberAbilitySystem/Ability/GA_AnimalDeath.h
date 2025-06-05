// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AnimalDeath.generated.h"

/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API UGA_AnimalDeath : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_AnimalDeath();

	UFUNCTION(BlueprintCallable)
	void CallEndAbility(const FGameplayTag Tag, int32 NewCount);
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION(BlueprintCallable)
	void EndFarmingTime();
	
	
	
	void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> DeathEffectToApply;

private:
	FTimerHandle WaitFarmingTimerHandle;
	FDelegateHandle EndCueDelegateHandle;
};
