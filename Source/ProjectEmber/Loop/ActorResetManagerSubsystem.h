// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoopEventListener.h"
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

	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void RegisterResetRequest(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	bool ShouldResetActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void ConsumeResetFlag(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void SetCurrentLoopID(int32 LoopID);
private:
	UPROPERTY()
	TMap<FGuid, int32> PendingResetActors;

	UPROPERTY()
	int32 CurrentLoop;
};

