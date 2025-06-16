// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AnimalSpeed.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MessageBus/MessageBus.h"

UGA_AnimalSpeed::UGA_AnimalSpeed()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_AnimalSpeed::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
	
	if (!TriggerEventData)
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
		return;
	}

	//종족 고유의 이동속도
	OriginSpeed = TriggerEventData->EventMagnitude;

	//스피드 업 게임플레이이팩트
	if (StartEffect)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(StartEffect, GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,
			FGameplayTag::RequestGameplayTag("Trigger.Animal.SpeedUp"),OriginSpeed*1.5f);
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
	}
	
	//타이머 걸기 -> ApplyTime초 빠른 속도 유지 후 감소
	GetWorld()->GetTimerManager().SetTimer(SpeedTimerHandle, this,
		&ThisClass::DownSpeedTime, ApplyTime, false);
}

void UGA_AnimalSpeed::DownSpeedTime()
{
	//스피드 다운 게임플레이이팩트
	if (EndEffect)
	{	float Boostspeed = OriginSpeed * 1.5f;
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EndEffect, GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,
			FGameplayTag::RequestGameplayTag("Trigger.Animal.SpeedDown"),-(Boostspeed-OriginSpeed)/20);
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(SpeedTimerHandle, this,
		&ThisClass::EndSpeedChangTime, 10.f, false);
}

void UGA_AnimalSpeed::EndSpeedChangTime()
{
	UObject* Animal = Cast<UObject>(GetAvatarActorFromActorInfo());
	UMessageBus::GetInstance()->BroadcastMessage(TEXT("FixSpeed"), Animal);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
