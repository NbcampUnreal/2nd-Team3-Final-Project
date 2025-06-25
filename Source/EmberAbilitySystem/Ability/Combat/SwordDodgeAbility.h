// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SwordDodgeAbility.generated.h"

/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API USwordDodgeAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USwordDodgeAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAnimMontage* ForwardDodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAnimMontage* BackDodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAnimMontage* LeftDodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAnimMontage* RightDodgeMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Warping")
	float WarpDistance{100.0f};
	
	UFUNCTION()
	void OnMontageFinished();

	void SetUpdateWarpingToTarget() const;

	void SetUpdateWarping(bool bIsForceRotation) const;
};
