// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LoopEventRegistryAsset.generated.h"
class ULoopActionBase;
class ULoopEventDataAsset;

/**
 * 
 */
UCLASS(BlueprintType)
class PROJECTEMBER_API ULoopEventRegistryAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TMap<int32, TSoftObjectPtr<ULoopEventDataAsset>> LoopEventMap;
};
