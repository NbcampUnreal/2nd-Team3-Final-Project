// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Condition/InteractionCondition.h"
#include "InteractionCondition_NotExceed.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UInteractionCondition_NotExceed : public UInteractionCondition
{
	GENERATED_BODY()
public:
	virtual bool IsFulfilled() const override;
};
