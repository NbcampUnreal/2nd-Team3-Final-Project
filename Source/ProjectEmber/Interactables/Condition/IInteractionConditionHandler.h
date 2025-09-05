// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IInteractionConditionHandler.generated.h"

/**
 * 
 */

struct FGameplayTag;

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractionConditionHandler : public UInterface
{
	GENERATED_BODY()
};

class PROJECTEMBER_API IInteractionConditionHandler
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void HandleInteractionEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);

};
