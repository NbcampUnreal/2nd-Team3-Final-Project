// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/EmberDataContainer.h"
#include "EmberDropItemManager.generated.h"


UCLASS(ClassGroup=(Custom))
class PROJECTEMBER_API UEmberDropItemManager : public UEmberDataContainer
{
	GENERATED_BODY()
	
public:
	TArray<FItemPair> GetDropItemName(const FName& MonsterID, const UAbilitySystemComponent* AbilitySystemComponent = nullptr);

	void SetDropItem(const FName& MonsterID, const UAbilitySystemComponent* AbilitySystemComponent = nullptr);
};
