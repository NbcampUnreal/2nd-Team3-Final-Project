// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Drop/EmberOreItemComponent.h"

#include "Character/EmberCharacter.h"
#include "Item/Ability/EmberItemAttributeSet.h"

bool UEmberOreItemComponent::bAbleDrop(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (AbilitySystemComponent)
	{
		if (const UEmberItemAttributeSet* ItemAttributeSet = Cast<UEmberItemAttributeSet>(AbilitySystemComponent->GetAttributeSet(UEmberItemAttributeSet::StaticClass())))
		{
			if (ItemAttributeSet->GetItemMiningLevel() >= MiningLevel)
			{
				return true;	
			}
		}
	}
	return false;
}

void UEmberOreItemComponent::DropItem(AActor* Actor)
{
	if (TObjectPtr<AEmberCharacter> Character = Cast<AEmberCharacter>(Actor))
	{
		if (bAbleDrop(Character->GetAbilitySystemComponent()))
		{
			Super::DropItem(Actor);
		}
	}
}
