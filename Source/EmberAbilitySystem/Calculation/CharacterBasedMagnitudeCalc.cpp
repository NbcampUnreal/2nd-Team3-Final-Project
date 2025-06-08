// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBasedMagnitudeCalc.h"
#include "GameplayEffectTypes.h"

float UCharacterBasedMagnitudeCalc::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const TMap<FGameplayTag, float>& TagToMagnitudeMap = Spec.SetByCallerTagMagnitudes;

	for (const TPair<FGameplayTag, float>& Elem : TagToMagnitudeMap)
	{
		if (Elem.Key.IsValid())
		{
			return Elem.Value;
		}
	}


	// 만약 루프를 다 돌았는데도 유효한 태그-값을 찾지 못했다면
	// 안전하게 0.0f를 반환합니다.
	return 0.0f;
}
