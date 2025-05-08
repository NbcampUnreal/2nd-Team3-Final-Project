// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_SwordComboAttack.generated.h"

/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API UGA_SwordComboAttack : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_SwordComboAttack();
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
							   const FGameplayAbilityActivationInfo ActivationInfo,
							   bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
							const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
							bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
							  const FGameplayAbilityActivationInfo ActivationInfo) override;

	FName GetNextSection();
	
protected:
	UFUNCTION()
	void OnCompleteCallback();
	
	UFUNCTION()
	void OnInterruptedCallback();

	void StartComboTimer();
	void CheckComboInput();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> SwordComboActionMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEmberComboActionData> CurrentComboData;
	
	uint8 CurrentComboIndex{0};
	bool HasNextComboInput{false};
	
	FTimerHandle ComboTimerHandle;
};
