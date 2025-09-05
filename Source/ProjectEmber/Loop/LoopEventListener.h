// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoopEventListener.generated.h"

class UGameLoopManagerSubsystem;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULoopEventListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTEMBER_API ILoopEventListener
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnLoopEnding(UGameLoopManagerSubsystem* Manager);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnLoopStarted(UGameLoopManagerSubsystem* Manager);

};
