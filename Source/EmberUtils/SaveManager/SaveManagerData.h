#pragma once

#include "CoreMinimal.h"
#include "EMSCustomSaveGame.h"
#include "SaveManagerData.generated.h"

/**
 * 
 */
UCLASS()
class EMBERUTILS_API USaveManagerData : public UEMSCustomSaveGame
{
	GENERATED_BODY()
	
public:
	USaveManagerData()
	{
		// Use the save slot system and set a custom file name
		bUseSaveSlot = true;
		SaveGameName = TEXT("SaveManagerData");
	}

	/**
	 * Generic key-value store (all values serialized as strings).
	 */
	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FString> DataMap;
};
