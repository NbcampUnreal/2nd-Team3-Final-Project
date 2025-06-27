#pragma once

#include "CoreMinimal.h"
#include "EmberAudioSettings.generated.h"

USTRUCT(BlueprintType)
struct FEmberAudioSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, SaveGame)
    float MasterVolume{1.f};

    UPROPERTY(BlueprintReadWrite, SaveGame)
    float BgmVolume{1.f};

    UPROPERTY(BlueprintReadWrite, SaveGame)
    float EffectsVolume{1.f};
};