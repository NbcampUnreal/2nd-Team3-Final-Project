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
	//HiddenAnimals는 특정 하나를 키로 삼아 꺼내는 용도가 아니라,
	//동물 객체가 이 집합에 포함되어 있는지만 빠르게 검사하면 되기 때문에,
	//중복 없이 빠른 검색/삽입/삭제가 가능한 TSet을 사용했습니다, 삽입삭제O(1)
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
