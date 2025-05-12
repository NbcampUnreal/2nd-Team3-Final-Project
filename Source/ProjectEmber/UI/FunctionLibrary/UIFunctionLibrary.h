// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIFunctionLibrary.generated.h"

struct FGameplayTag;
class UEmberLayerBase;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static bool RegisterLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag, UEmberLayerBase* Layer);
};
