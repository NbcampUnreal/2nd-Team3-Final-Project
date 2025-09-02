// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Loop/LoopAction/Fragment/LoopActionFragment.h"
#include "LoopActionFragment_Bool.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API ULoopActionFragment_Bool : public ULoopActionFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Value;
};
