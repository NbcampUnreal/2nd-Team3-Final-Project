#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EmberComboActionData.generated.h"

UCLASS()
class EMBERABILITYSYSTEM_API UEmberComboActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UEmberComboActionData();

	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix;

	UPROPERTY(EditAnywhere, Category = Name)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category = Name)
	float FrameRate;

	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;
};
