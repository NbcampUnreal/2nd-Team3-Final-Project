#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RollingStone.generated.h"

class USphereComponent;

UCLASS()
class PROJECTEMBER_API ARollingStone : public AActor
{
	GENERATED_BODY()
	
public:	
	ARollingStone();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
					  AActor* OtherActor,
					  UPrimitiveComponent* OtherComp,
					  int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	USphereComponent* DeathTrigger;
};
