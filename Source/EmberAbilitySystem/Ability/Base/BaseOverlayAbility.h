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

	/**
	 * Force Tags
	 * AlsCharacter에게 강제된 행동을 주는데 쓰임
	 * 이 어빌리티는 현재 OwnedTag로 Als.Gait.Running.Lock 태그를 달고 직접
	 * AlsCharacter에 행동을 막고 있음 CanChangeGait 라는 함수에서
	 * 본인이 하려는 Desired를 막음과 동시에 ForceGameplayTags가 있으면 그 행동을 취함
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer ForceGameplayTags;
	
	/**
	 * Launch Character
	 * 루트모션이 없어도 플레이어가 키를 누르고 있으면 몽타주 재생중에 앞으로 가는 느낌을
	 * 줄려고 고안했음 하지만 제대로 작동안함 다른 방식을 취해야 할거같음
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement")
	bool bEnableForwardMovementDuringMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement", meta = (EditCondition = "bEnableForwardMovementDuringMontage"))
	float ForwardMovementDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement", meta = (EditCondition = "bEnableForwardMovementDuringMontage"))
	float ForwardMovementDuration;

	/**
	 * 나중에 태그 기반 여러 어빌리티를 호출하고 싶어지면 키자
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

	/**
	 * 어빌리티가 발동 중일때 한번 더 키가 눌렸는지 체크
	 * 아직 NotifyState를 만들지 않아서 정확히는
	 * 다음 콤보 어빌리티가 발동되는 노티파이(AnimNotify_PlayGameplayEvent)
	 * 전에 눌리면 무조건 호출함
	 */
	bool bComboInputReceived = false;

	/**
	 * 루트모션 오류에 대한 테스트
	 * 추후 문제가 생길시 FAlsLocomotionState를 제거하고
	 * 그에 맞는 상태를 찾아봐야 함
	 * (TargetYawAngle 쪽 값을 강제 시켜야 할수도 있음)
	 */
	float StartMontageActorYaw = 0.0f;
	FAlsLocomotionState PreLocomotionState;
	FVector PreDirection;
	FTimerHandle MontageTickHandle;
};
