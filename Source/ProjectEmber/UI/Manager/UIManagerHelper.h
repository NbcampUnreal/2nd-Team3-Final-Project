#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIManager.h"
#include "UIManagerHelper.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UUIManagerHelper : public UObject
{
	GENERATED_BODY()
	
public:
	TMap<ELayer, TSubclassOf<UEmberBaseWidget>> WidgetClasses;
};
