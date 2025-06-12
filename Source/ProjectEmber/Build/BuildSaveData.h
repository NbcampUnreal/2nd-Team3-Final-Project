// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "BuildSaveData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FBuildSaveData
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    FString ActorClassPath;

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    FVector Location;

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    FRotator Rotation;

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    FVector Scale;
};