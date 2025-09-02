// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Loop/LoopAction/LoopActionBase.h"
#include "LoopAction_SetTimerPause.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API ULoopAction_SetTimerPause : public ULoopActionBase
{
	GENERATED_BODY()
public:
	virtual void ActivateAction_Implementation() override;
};
