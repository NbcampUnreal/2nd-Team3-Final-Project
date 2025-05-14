#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../EnumSet.h"
#include "UISfxAudioDataStruct.generated.h"

USTRUCT(BlueprintType)
struct PROJECTEMBER_API FUISFXAudioDataStruct : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUISfxSoundType UISfxSoundType = EUISfxSoundType::Click;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> Sound = nullptr;
};
