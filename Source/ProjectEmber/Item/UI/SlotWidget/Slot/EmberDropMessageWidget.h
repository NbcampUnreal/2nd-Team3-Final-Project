// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseSlotWidget.h"
#include "EmberDropMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberDropMessageWidget : public UEmberBaseSlotWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StartShowTime();
	UFUNCTION(BlueprintCallable)
	void EndShowTime();

	virtual bool bCanVisible() override;
protected:
	FTimerHandle TimerHandle;
};
