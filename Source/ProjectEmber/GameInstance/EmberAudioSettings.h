#pragma once

#include "CoreMinimal.h"
#include "EmberAudioSettings.generated.h"

USTRUCT(BlueprintType)
struct FEmberAudioSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, SaveGame)
    float MasterVolume;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    float BgmVolume;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    float EffectsVolume;
};