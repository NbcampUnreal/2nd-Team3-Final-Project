// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "BuildableData.generated.h"
/**
 *
 */
USTRUCT(BlueprintType)
struct FBuildableData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* Mesh{nullptr};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<ETraceTypeQuery> TraceChannel{TraceTypeQuery1};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> Actor;

    // 자원과 자원 종류, 건물 체력  추가

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	FName RequiredResources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredResourceAmounts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText BuildableDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail;

};
