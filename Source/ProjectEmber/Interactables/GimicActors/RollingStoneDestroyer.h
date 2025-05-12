#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RollingStoneDestroyer.generated.h"

class ARollingStoneSpawner;
class UBoxComponent;

UCLASS()
class PROJECTEMBER_API ARollingStoneDestroyer : public AActor
{
	GENERATED_BODY()
	
public:	
	ARollingStoneDestroyer();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

public:
	// 에디터에서 스포너 인스턴스 할당해줘야됨 안하면 에러남
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner")
	ARollingStoneSpawner* Spawner;

private:
	TWeakObjectPtr<UBoxComponent> DestroyerBox;
};
