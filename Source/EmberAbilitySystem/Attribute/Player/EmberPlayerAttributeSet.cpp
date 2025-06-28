// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberPlayerAttributeSet.h"

UEmberPlayerAttributeSet::UEmberPlayerAttributeSet()
{
	InitParryDuration(0.3f);
	InitPerfectDodgeDuration(0.1f);
}

void UEmberPlayerAttributeSet::Initialize(UAbilitySystemComponent* InAsc)
{
	EffectHelperInstance = NewObject<UEmberEffectHelper>(GetTransientPackage(), EffectHelperClass);
}

void UEmberPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UEmberPlayerAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

bool UEmberPlayerAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void UEmberPlayerAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UEmberPlayerAttributeSet::ApplyGameplayEffectToSelf(UAbilitySystemComponent* AbilitySystemComponent,
                                                         const TSubclassOf<UGameplayEffect>& EffectClass, float Level)
{
	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	/* 나중에 들어온 Data로 커브값을 세팅해서 증가시키면 강도에 따라 마나 차는게 조절이 가능함*/
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level,Context);
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}
