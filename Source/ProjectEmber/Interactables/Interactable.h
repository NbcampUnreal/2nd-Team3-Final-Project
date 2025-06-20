#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTEMBER_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void TryInteract(AActor* Caller);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void Interact(AActor* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void EndInteract();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	float GetGatherTime();
};
