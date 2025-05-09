#pragma once

#include "CoreMinimal.h"
#include "SoundDataArrayStruct.generated.h"

struct FAreaAudioDataStruct;
struct FCharacterAudioDataStruct;

USTRUCT(BlueprintType)
struct PROJECTEMBER_API FSoundDataArrayStruct
{
	GENERATED_BODY()

public:
	TArray<FAreaAudioDataStruct*> AreaAudioArray;
	TArray<FCharacterAudioDataStruct*> CharacterAudioArray;
};
