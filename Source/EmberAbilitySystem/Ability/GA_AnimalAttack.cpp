// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AnimalAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_AnimalAttack::UGA_AnimalAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_AnimalAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
										  const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo,
										  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
	
	if (!TriggerEventData)
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
		return;
	}
	
	if (OwnerInfo->OwnerActor.IsValid())
	{
		Instigator = OwnerInfo->OwnerActor;
	}

	if (TriggerEventData->OptionalObject)
	{
		Montage = const_cast<UAnimMontage*>(Cast<const UAnimMontage>(TriggerEventData->OptionalObject.Get()));
	}
	ensure(Montage);
	
	// 몽타주 재생
	UAbilityTask_PlayMontageAndWait* Task =	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("AnimalAttack"), Montage);
	if (Task)
	{
		Task->OnCompleted.AddDynamic(this, &UGA_AnimalAttack::OnCompleteCallback);
		Task->ReadyForActivation();
	}
}

void UGA_AnimalAttack::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
