#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Gimic.generated.h"

UINTERFACE(MinimalAPI)
class UGimic : public UInterface
{
	GENERATED_BODY()
};

class PROJECTEMBER_API IGimic
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void PlayGimic();
};
