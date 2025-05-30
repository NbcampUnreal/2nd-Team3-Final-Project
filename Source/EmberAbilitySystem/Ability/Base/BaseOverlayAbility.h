#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "State/AlsLocomotionState.h"
#include "BaseOverlayAbility.generated.h"

/**
 * 기존 몽타주 대신 ALS state에 따라 다른 몽타주를 재생시키기 위한 구조체
 */
USTRUCT(BlueprintType)
struct EMBERABILITYSYSTEM_API FStateMontageMapping
{
	GENERATED_BODY()

	/**
	 * AbilitySystemComponent가 가진 StateTag와 매핑되는 태그
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|StateMapping")
	FGameplayTag StateTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|StateMapping")
	UAnimMontage* Montage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|StateMapping")
	int32 Priority = 0;
};

USTRUCT(BlueprintType)
struct EMBERABILITYSYSTEM_API FStateAbilityMapping
{
	GENERATED_BODY()

	/**
	 * AbilitySystemComponent가 가진 StateTag와 매핑되는 태그
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|StateMapping")
	FGameplayTag StateTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|StateMapping")
	TSubclassOf<UGameplayAbility> Ability;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|StateMapping")
	int32 Priority = 0;
};

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
	/** Default Montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DefaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base")
	bool bLoopingMontage;

	/**
	 * Als 캐릭터에게 이 태그가 있는지 체크 후
	 * 있다면 DefaultMontage대신 재생
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base")
	TArray<FStateMontageMapping> StateMontageMappings;

	/**
	 * Als 캐릭터에게 이 태그가 있는지 체크 후
	 * 있다면 EndAbility이후에 호출
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Base")
	TArray<FStateAbilityMapping> StateAbilityMappings;
	
	
	/** Combo Montage */
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
	 * 특정 몽타주 재생 후 VelocityYawAngle 쪽이 틀어져서 반대방향을 바라보게 됨
	 * AlsMovementComponent는 Tick마다 바라볼 다양한 YawAngle값으로 플레이어를 회전시켰고
	 * 그로 인해 끝나고 난 뒤 플레이어가 무조건 뒤를 보며 회전하는걸 확인
	 * 임시방편으로 Tick처럼 플레이어가 재생을 시작하는 방향을 담은 LocomotionState를 저장
	 * 어빌리티가 끝날 때 까지 Tick에서 덮어쓰고 있음
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Movement")
	bool bMontageTickEnable{false};

protected:
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();
	UFUNCTION()
	void OnComboNotify(const FGameplayEventData Payload);
	UFUNCTION()
	void OnMontageTick() const;
	UFUNCTION()
	void OnBlendOut();
private:
	void LaunchCharacterForward(const FGameplayAbilityActorInfo* ActorInfo) const;
	
	UAnimMontage*					ChooseMontageByState();
	TSubclassOf<UGameplayAbility>	ChooseAbilityByState();
	
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
