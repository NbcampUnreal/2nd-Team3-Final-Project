#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../EnumSet.h"
#include "CharacterAudioDataStruct.generated.h"

USTRUCT(BlueprintType)
struct PROJECTEMBER_API FCharacterAudioDataStruct : public FTableRowBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> Sound = nullptr;
};
