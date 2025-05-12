#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RollingStoneSpawner.generated.h"

class ARollingStone;

UCLASS()
class PROJECTEMBER_API ARollingStoneSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ARollingStoneSpawner();

protected:
	virtual void BeginPlay() override;

public:
	void SetIsSpawning(const bool isSpawning);
	bool GetIsSpawning() const;
	void SpawnRollingStone();
	void DestroyRollingStone();
	bool bIsSpawning = false;

	UPROPERTY(EditAnywhere, Category = "Stone")
	TSubclassOf<AActor> RollingStoneClass;

private:

	UPROPERTY(VisibleAnywhere, Category = "Stone")
	ARollingStone* RollingStone = nullptr;
};
