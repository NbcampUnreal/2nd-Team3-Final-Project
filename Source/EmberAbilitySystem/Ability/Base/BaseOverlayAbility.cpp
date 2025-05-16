// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseOverlayAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"

UBaseOverlayAbility::UBaseOverlayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBaseOverlayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	/* 모든 행동에대한 코스트, 쿨타임 등을 한번에 처리하고 bool 리턴해줌 */
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	LaunchCharacterForward(ActorInfo);
	
	AbilitySystemComponent = CastChecked<UAbilitySystemComponent>(ActorInfo->AvatarActor.Get()->GetComponentByClass(UAbilitySystemComponent::StaticClass()));

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, MontageToPlay);

	PlayMontageTask->OnCompleted.AddDynamic(this, &UBaseOverlayAbility::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UBaseOverlayAbility::OnMontageInterrupted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UBaseOverlayAbility::OnMontageInterrupted);
	PlayMontageTask->ReadyForActivation();
}

void UBaseOverlayAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UBaseOverlayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBaseOverlayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	bComboInputReceived = true;
}

void UBaseOverlayAbility::OnMontageCompleted()
{
	if (bCanCombo && NextComboTag.IsValid() && bComboInputReceived)
	{
		// Tag에 매핑된 Ability를 ASC 에서 찾아서 실행
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(NextComboTag), true);
	}
	
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBaseOverlayAbility::OnMontageInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBaseOverlayAbility::LaunchCharacterForward(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (bEnableForwardMovementDuringMontage && ForwardMovementDistance > 0.f && ForwardMovementDuration > 0.f)
	{
		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			const FVector Forward = Character->GetActorForwardVector();
			const float Speed = ForwardMovementDistance / ForwardMovementDuration;
			Character->LaunchCharacter(Forward * Speed, true, true);
		}
	}
}
