#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ELevelType : uint8
{
	LobbyLevel = 0,
	FieldLevel
};

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

UENUM(BlueprintType)
enum class ESfxSoundType : uint8
{
	UI = 0,
	Monster,
	Character
};

UENUM(BlueprintType)
enum class EUISfxSoundType : uint8
{
	Click = 0
};
