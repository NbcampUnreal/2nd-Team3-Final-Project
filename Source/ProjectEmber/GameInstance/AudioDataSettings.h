#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AudioDataSettings.generated.h"

UCLASS(Config = AudioDataSettings, DefaultConfig, meta = (DisplayName = "AudioData Settings"))
class PROJECTEMBER_API UAudioDataSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Sounds")
	TSoftObjectPtr<UDataTable> AreaSounds;

	UPROPERTY(EditAnywhere, Config, Category = "Sounds")
	TSoftObjectPtr<UDataTable> CharacterSounds;

	UPROPERTY(EditAnywhere, Config, Category = "Sounds")
	TSoftObjectPtr<UDataTable> UISfxSounds;
};
