#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "State/AlsLocomotionState.h"
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
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	/* Montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base")
	bool bLoopingMontage;

	/* Combo Montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	bool bCanCombo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayAbility> NextComboAbility;

	/* Force Tags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer ForceGameplayTags;
	
	/* Launch Character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement")
	bool bEnableForwardMovementDuringMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement", meta = (EditCondition = "bEnableForwardMovementDuringMontage"))
	float ForwardMovementDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement", meta = (EditCondition = "bEnableForwardMovementDuringMontage"))
	float ForwardMovementDuration;

	/* 나중에 태그 기반 여러 어빌리티를 호출하고 싶어지면 키자
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	FGameplayTag NextComboTag;*/
protected:
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnComboNotify(const FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageTick() const;
private:
	void LaunchCharacterForward(const FGameplayAbilityActorInfo* ActorInfo) const;
	
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	bool bComboInputReceived = false;

	float StartMontageActorYaw = 0.0f;
	FAlsLocomotionState PreLocomotionState;
	FVector PreDirection;
	FTimerHandle MontageTickHandle;
};
