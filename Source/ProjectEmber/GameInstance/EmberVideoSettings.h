#pragma once

#include "CoreMinimal.h"
#include "EmberVideoSettings.generated.h"

USTRUCT(BlueprintType)
struct FEmberVideoSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FString Resolution;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FString ScreenMode;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FString OverallQuality;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FString TextureQuality;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FString ShadowQuality;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FString EffectsQuality;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FString AntiAliasingQuality;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FString ViewDistanceQuality;
};