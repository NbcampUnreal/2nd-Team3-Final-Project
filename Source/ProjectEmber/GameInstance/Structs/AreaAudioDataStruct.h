#pragma once

#include "CoreMinimal.h"
#include "GameInstance/EnumSet.h"
#include "Engine/DataTable.h"
#include "AreaAudioDataStruct.generated.h"

USTRUCT(BlueprintType)
struct PROJECTEMBER_API FAreaAudioDataStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAreaSoundType AreaSoundType = EAreaSoundType::LobbySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> Sound = nullptr;
};



