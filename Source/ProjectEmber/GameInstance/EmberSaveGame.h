#pragma once

#include "CoreMinimal.h"
#include "Character/InputHandler/EmberKeyMappingEntry.h"
#include "EasyMultiSave/Public/EMSCustomSaveGame.h"
#include "EmberSaveGame.generated.h"

UCLASS()
class PROJECTEMBER_API UEmberSaveGame : public UEMSCustomSaveGame
{
	GENERATED_BODY()

public:

    UPROPERTY()
    TArray<FEmberKeyMappingEntry> SavedMappings;

    UPROPERTY()
    TArray<FEmberDirectionalMoveEntry> SavedMoveBindings;
	
};
