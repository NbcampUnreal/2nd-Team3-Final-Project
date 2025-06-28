// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "EmberItemAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)	   \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)			   \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)			   \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)\

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberItemAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UEmberItemAttributeSet, ItemDropRate)
	ATTRIBUTE_ACCESSORS(UEmberItemAttributeSet, ItemDropBonus)
	ATTRIBUTE_ACCESSORS(UEmberItemAttributeSet, ItemMiningLevel)

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData ItemDropRate;

	UPROPERTY(BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData ItemDropBonus;
	
	UPROPERTY(BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData ItemMiningLevel;
	
};
