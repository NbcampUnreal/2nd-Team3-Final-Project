#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAreaType : uint8 
{
	LobbyArea = 0,
	GrasslandArea,
	DesertArea
};

UENUM(BlueprintType)
enum class EAreaSoundType : uint8 
{
	LobbySound = 0,
	GrasslandSound,
	DesertSound
};

UENUM(BlueprintType)
enum class ECharacterSoundType : uint8
{
	None = 0
};
