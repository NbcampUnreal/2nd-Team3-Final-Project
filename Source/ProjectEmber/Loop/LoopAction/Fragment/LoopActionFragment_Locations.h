// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Loop/LoopAction/Fragment/LoopActionFragment.h"
#include "LoopActionFragment_Locations.generated.h"

class ARespawnTarget;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTEMBER_API ULoopActionFragment_Locations : public ULoopActionFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<ARespawnTarget> RespawnTarget;
};
