// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LoopEventSettings.generated.h"

class ULoopEventDataAsset;
class ULoopActionBase;

USTRUCT(BlueprintType)
struct FLoopActionEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULoopActionBase> ActionClass;

	UPROPERTY(EditAnywhere)
	ULoopEventDataAsset* ActionDA;
};

/**
 * 
 */
UCLASS(Config = Game, meta=(DisplayName="Loop Action Settings"))
class PROJECTEMBER_API ULoopEventSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<int32, TObjectPtr<ULoopEventDataAsset>> LoopEventMap;

	UPROPERTY(EditAnywhere, Config)
	int32 DefaultLoopID = -1;
};
