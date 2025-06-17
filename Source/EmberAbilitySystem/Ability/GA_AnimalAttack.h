// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AnimalAttack.generated.h"

UCLASS()
class EMBERABILITYSYSTEM_API UGA_AnimalAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_AnimalAttack();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AnimalAttack")
	UAnimMontage* Montage;

protected:

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> EffectToApply;
	
private:
	UFUNCTION()
	void OnCompleteCallback();
	UFUNCTION()
	void OnMontageInterrupted();
	
	
	TWeakObjectPtr<const AActor> Instigator;
	bool IsSpecialAttack = false;
};