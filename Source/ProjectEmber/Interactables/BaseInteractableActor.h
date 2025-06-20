#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "BaseInteractableActor.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTEMBER_API ABaseInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ABaseInteractableActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TryInteract_Implementation(AActor* Caller) override;
	virtual void Interact_Implementation(AActor* Caller) override;
	virtual float GetGatherTime_Implementation() override; 


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	FGameplayTagContainer GameplayTagContainer;
};
