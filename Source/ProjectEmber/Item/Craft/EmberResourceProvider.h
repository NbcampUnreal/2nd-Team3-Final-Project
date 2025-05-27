// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/EmberCraftStruct.h"
#include "UObject/Interface.h"
#include "Item/Core/EmberItemStruct.h"
#include "EmberResourceProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UEmberResourceProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTEMBER_API IEmberResourceProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TMap<FName, int32> GetAllItemInfos();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TryConsumeResource(const TArray<FItemPair>& InRequireItems);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FItemPair> RemoveResourceUntilAble(const TArray<FItemPair>& InRequireItems);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool bConsumeAbleResource(const TArray<FItemPair>& InRequireItems);

};
