#pragma once

#include "CoreMinimal.h"
#include "GameInstance/EnumSet.h"
#include "GameFramework/Actor.h"
#include "AreaTrigger.generated.h"

class UEmberGameInstance;
class UAudioSubsystem;
class UBoxComponent;

UCLASS()
class PROJECTEMBER_API AAreaTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AAreaTrigger();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Interactable")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Area")
	EAreaType AreaType;

	TWeakObjectPtr<class UAudioSubsystem> AudioHandle;
	TWeakObjectPtr<class UEmberGameInstance> GI;
};
