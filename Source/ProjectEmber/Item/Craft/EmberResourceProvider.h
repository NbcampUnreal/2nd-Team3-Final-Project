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
	void GetItemInfo(FEmberItemEntry& InItemEntry, FInstancedStruct& OutItemInfo);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetItemInfos(TArray<FEmberItemEntry>& InItemEntry, TMap<FEmberItemKey, FInstancedStruct>& OutItemInfos);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TryConsumeResource(const TArray<FEmberItemEntry>& InRequireItems);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveResourceUntilAble(TArray<FEmberItemEntry>& InRequireItems);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool bConsumeAbleResource(const TArray<FEmberItemEntry>& InRequireItems);

};
