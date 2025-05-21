// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnimalSpawner.generated.h"

class ABaseAIAnimal;
class AAnimalSpawnPoint;

USTRUCT(BlueprintType)
struct FAnimalSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseAIAnimal> AnimalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LeaderCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PatrolCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FollowCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AloneCount = 0;

	// 팔로워 수는 TotalCount - LeaderCount - PatrolCount
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TSoftObjectPtr<ABaseAIAnimal>> SpawnAnimals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TSoftObjectPtr<ABaseAIAnimal>> HiddenAnimals;
};

UCLASS()
class PROJECTEMBER_API AAnimalSpawner : public AActor
{
	GENERATED_BODY()

public:
	AAnimalSpawner();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp,
							 AActor* OtherActor,
							 UPrimitiveComponent* OtherComp,
							 int32 OtherBodyIndex,
							 bool bFromSweep,
							 const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void TrySpawnAnimals();


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	class UBoxComponent* BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TArray<FAnimalSpawnInfo> AnimalsInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TArray<TSoftObjectPtr<AAnimalSpawnPoint>> SpawnPoints;
};
