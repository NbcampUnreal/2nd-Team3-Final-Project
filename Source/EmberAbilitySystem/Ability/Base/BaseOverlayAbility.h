#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseOverlayAbility.generated.h"

/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API UBaseOverlayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UBaseOverlayAbility();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MontageToPlay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	bool bCanCombo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	FGameplayTag NextComboTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement")
	bool bEnableForwardMovementDuringMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement", meta = (EditCondition = "bEnableForwardMovementDuringMontage"))
	float ForwardMovementDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement", meta = (EditCondition = "bEnableForwardMovementDuringMontage"))
	float ForwardMovementDuration;
	
protected:
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();

private:
	void LaunchCharacterForward(const FGameplayAbilityActorInfo* ActorInfo) const;
	
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	bool bComboInputReceived = false;
};
