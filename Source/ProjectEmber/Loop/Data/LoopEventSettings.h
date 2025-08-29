// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LoopEventSettings.generated.h"

class ULoopEventDataAsset;
class ULoopEventRegistryAsset;

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta=(DisplayName="Loop Action Settings"))
class PROJECTEMBER_API ULoopEventSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category="Loop", meta=(AllowedClasses="LoopEventDataAsset"))
	TMap<int32, TSoftObjectPtr<ULoopEventDataAsset>> LoopDatas;
	
	UPROPERTY(EditAnywhere, Config)
	int32 DefaultLoopID = -1;
};
