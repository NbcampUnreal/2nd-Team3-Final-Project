// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHitAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UBaseHitAbility::UBaseHitAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBaseHitAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (HitMontage)
	{
		auto* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HitMontage);

		PlayMontageTask->OnCompleted.AddDynamic(this, &UBaseHitAbility::OnMontageFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UBaseHitAbility::OnMontageFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UBaseHitAbility::OnMontageFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UBaseHitAbility::OnMontageFinished);

		PlayMontageTask->ReadyForActivation();
		return;	
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
}

void UBaseHitAbility::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
