// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoopEventDirector.generated.h"

class ULoopActionDefinition;
class ULoopActionBase;
struct FLoopActionDefinition;
class UGameLoopManagerSubsystem;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API ULoopEventDirector : public UObject
{
	GENERATED_BODY()

public:
	void RunStartActions(UGameLoopManagerSubsystem* InManager, int32 LoopID);
	void RunEndActions(UGameLoopManagerSubsystem* InManager, int32 LoopID);
	
	void OnActionFinished(ULoopActionBase* FinishedAction);

	UFUNCTION(BlueprintCallable)
	UWorld* GetWorldContext() const;
protected:
	void RunActionsForLoop(int32 LoopID);
	void ExecuteActionDefinitions(const TArray<TObjectPtr<ULoopActionDefinition>>& Definitions);
	
	UPROPERTY()
	TArray<TObjectPtr<ULoopActionBase>> PendingActions;

	UPROPERTY()
	TObjectPtr<UGameLoopManagerSubsystem> CachedLoopManager;

	UPROPERTY()
	bool bIsStart = false;
};


