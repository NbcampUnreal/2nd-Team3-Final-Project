// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ActorResetManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UActorResetManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void RegisterResetRequest(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	bool ShouldResetActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void ConsumeResetFlag(AActor* Actor);

private:
	UFUNCTION()
	void LoopStarted(int32 LoopIndex);
	
	UPROPERTY()
	TMap<FGuid, int32> PendingResetActors;

	int32 CurrentLoop = 0;
};

