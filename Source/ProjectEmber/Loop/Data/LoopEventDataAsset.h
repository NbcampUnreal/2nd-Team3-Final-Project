// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LoopEventDataAsset.generated.h"

class ULoopActionFragment;
class ULoopActionBase;

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class PROJECTEMBER_API ULoopActionDefinition : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ULoopActionBase> ActionClass;

	UPROPERTY(EditAnywhere, Instanced, Category = "Action", meta = (ShowInnerProperties))
	TArray<TObjectPtr<ULoopActionFragment>> Fragments;
};
/**
 * 
 */
UCLASS(BlueprintType)
class PROJECTEMBER_API ULoopEventDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=LoopActions)
	TArray<TObjectPtr<ULoopActionDefinition>> StartActions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=LoopActions)
	TArray<TObjectPtr<ULoopActionDefinition>> EndActions;
};
