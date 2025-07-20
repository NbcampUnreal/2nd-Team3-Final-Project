// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLoopManagerSubsystem.generated.h"

class ULoopEventDirector;
class ILoopEventListener;

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UGameLoopManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	int32 GetCurrentLoopID() const {return CurrentLoopID;}
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void AdvanceLoop();
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void NotifyLoopEndReady();

	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void NotifyLoopStartReady();

private:
	void OnAllEndLoopReady();
	void OnAllStartLoopReady();

	UPROPERTY()
	int32 CurrentLoopID = 0;

	UPROPERTY()
	TObjectPtr<ULoopEventDirector> LoopDirector;

};
