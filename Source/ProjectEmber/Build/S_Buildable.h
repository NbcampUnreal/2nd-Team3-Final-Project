// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "S_Buildable.generated.h"
/**
 *
 */
USTRUCT(BlueprintType)
struct FBuildableData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<ETraceTypeQuery> TraceChannel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> Actor;
};