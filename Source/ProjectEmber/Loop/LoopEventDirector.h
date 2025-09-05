// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameLoopManagerSubsystem.h"
#include "LoopEventDirector.generated.h"

class ULoopEventDataAsset;
class ULoopActionDefinition;
class ULoopActionBase;
struct FLoopActionDefinition;
/**
 * 
 */

UCLASS()
class PROJECTEMBER_API ULoopEventDirector : public UObject
{
	GENERATED_BODY()

public:
	ULoopEventDirector();
	
	void RunStartActions(UGameLoopManagerSubsystem* InManager, int32 LoopID);
	void RunMidActions(UGameLoopManagerSubsystem* InManager, int32 LoopID);
	void RunEndActions(UGameLoopManagerSubsystem* InManager, int32 LoopID);
	
	void OnActionFinished(ULoopActionBase* FinishedAction);

	UFUNCTION(BlueprintCallable)
	UWorld* GetWorldContext() const;
protected:
	void RunActionsForLoop(int32 LoopID);
	void ExecuteActionDefinitions(const TArray<TObjectPtr<ULoopActionDefinition>>& Definitions);

	void LoadAllEvents();
	
	UPROPERTY()
	TArray<TObjectPtr<ULoopActionBase>> PendingActions;

	UPROPERTY()
	TObjectPtr<UGameLoopManagerSubsystem> CachedLoopManager;

	UPROPERTY()
	TMap<int32, TObjectPtr<ULoopEventDataAsset>> LoadedEvents;

	UPROPERTY()
	int32 DefaultEventID;
	
	UPROPERTY()
	ELoopEventPhase CurrentPhase = ELoopEventPhase::Start;

	UPROPERTY()
	int FinishedCount = 0;
};



