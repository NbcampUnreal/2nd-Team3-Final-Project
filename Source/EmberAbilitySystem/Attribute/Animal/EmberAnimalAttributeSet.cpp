// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberAnimalAttributeSet.h"

UEmberAnimalAttributeSet::UEmberAnimalAttributeSet()
{
	/* Behavior Tree */
	InitBumperRange(100.0f);
	InitWarningRange(150.0f);
	InitWalkSpeed(300.0f);
	InitWanderRange(500.0f);
	InitWildPower(0.0f);
}

void UEmberAnimalAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
}

void UEmberAnimalAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

bool UEmberAnimalAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void UEmberAnimalAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
