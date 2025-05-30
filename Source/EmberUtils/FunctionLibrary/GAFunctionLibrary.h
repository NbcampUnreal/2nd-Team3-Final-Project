// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GAFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMBERUTILS_API UGAFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UGAFunctionLibrary")
	static void HandleAnimalDeath(AActor* Animal);
};
