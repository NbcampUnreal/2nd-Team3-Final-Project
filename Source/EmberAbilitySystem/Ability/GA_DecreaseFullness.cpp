// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_DecreaseFullness.h"

#include "AbilitySystemComponent.h"

UGA_DecreaseFullness::UGA_DecreaseFullness()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_DecreaseFullness::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);

	if (EffectToApply)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectToApply, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle EffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
	}

	// AActor* Actor = GetAvatarActorFromActorInfo();
	// FGameplayCueParameters Param;
	// Param.SourceObject = this;
	// Param.Instigator = Actor;
	// Param.Location = Actor->GetActorLocation();
	//
	// GetAbilitySystemComponentFromActorInfo_Ensured()->
	// 	ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Animal.Decrease"), Param);

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	
}
