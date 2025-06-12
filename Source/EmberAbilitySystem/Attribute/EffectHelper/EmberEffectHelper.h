#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UObject/Object.h"
#include "EmberEffectHelper.generated.h"

UCLASS(Blueprintable)
class EMBERABILITYSYSTEM_API UEmberEffectHelper : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> ParryEffectClass;
};
