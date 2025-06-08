// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "CharacterBasedMagnitudeCalc.generated.h"
/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API UCharacterBasedMagnitudeCalc : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
