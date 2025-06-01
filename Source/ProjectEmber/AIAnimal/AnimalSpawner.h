// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EMSActorSaveInterface.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "MessageBus/MessageBus.h"
#include "AnimalSpawner.generated.h"

class ABaseAIAnimal;
class AAnimalSpawnPoint;

USTRUCT(BlueprintType)
struct FAnimalInitInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;
};

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
	//HiddenAnimals는 특정 하나를 키로 꺼내는 용도가 아니라,
	//동물 객체가 이 집합에 포함되어 있는지만 빠르게 검사하면 되기 때문에,
	//중복 없이 빠른 검색/삽입/삭제가 가능한 TSet을 사용했습니다, 삽입삭제O(1)
};

USTRUCT(BlueprintType)
struct FAnimalQueueInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseAIAnimal> AnimalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAnimalInitInfo InitInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RoleTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnInfoIndex = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnPointIndex = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SavedId;
};


UCLASS()
class PROJECTEMBER_API AAnimalSpawner : public AActor, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	AAnimalSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	//EMS
	virtual void ActorPreSave_Implementation() override;
	virtual void ActorLoaded_Implementation() override;
	
	//메세지버스
	void ReceiveMessage(const FName MessageType, UObject* Payload);
	void MessageMoveToHidden(UObject* Payload);
	FMessageDelegate MessageDelegateHandle;
	
	//Filter
	UFUNCTION(BlueprintCallable)
	FAnimalInitInfo GetRandomLocationInSpawnVolume(TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint);
	
	UFUNCTION(BlueprintCallable)
	void DistanceCheck();

	UFUNCTION(BlueprintCallable)
	TArray<TSoftObjectPtr<AAnimalSpawnPoint>> DiscardNearestPoint();

	UFUNCTION(BlueprintCallable)
	TArray<TSoftObjectPtr<AAnimalSpawnPoint>> SelectNearPoints(TArray<TSoftObjectPtr<AAnimalSpawnPoint>>& InSpawnPoints);
	
	//디스폰은 동물 마리 단위, 먼 순서대로 스폰되어 있는 모든 동물 정렬, 나중에 최적화? 시야범위 관련 사용자 설정에 쓰일 함수들
	UFUNCTION(BlueprintCallable)
	void SortFarthestAnimal();
	
	//Create
	UFUNCTION(BlueprintCallable)
	void TryCreateEntire(TArray<TSoftObjectPtr<AAnimalSpawnPoint>>& InSpawnPoints);

	UFUNCTION(BlueprintCallable)
	void CreateAnimalsQueue(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& InSpawnPoint);

	UFUNCTION(BlueprintCallable)
	void TryCreateQueue(TArray<TSoftObjectPtr<AAnimalSpawnPoint>>& InSpawnPoints);

	UFUNCTION(BlueprintCallable)
	void AddCreateQueue(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint, int32 Count, FName RoleTag);
	
	void TickCreateQueue(TQueue<FAnimalQueueInfo>& InQueue, bool& InIsLoading);

	
	//일괄
	UFUNCTION(BlueprintCallable)
	void CreateAnimals(FAnimalSpawnInfo& Info,TSoftObjectPtr<AAnimalSpawnPoint>& InSpawnPoint);

	UFUNCTION(BlueprintCallable)
	void SpawnAnimalWithTag(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint, FName RoleTag,
	                         int32 Count);

	//Spawn
	UFUNCTION(BlueprintCallable)  //전체 스폰
	void TrySpawnEntire();

	UFUNCTION(BlueprintCallable) //살아있는 애들만 스폰
	void TrySpawnAlive(TArray<FAnimalSpawnInfo>& InfoArray);
	
	UFUNCTION(BlueprintCallable) //죽은 애들만 스폰
	void TrySpawnDead(TArray<FAnimalSpawnInfo>& InfoArray);

	UFUNCTION(BlueprintCallable)
	void TickSpawnQueue();

	//Despawn
	UFUNCTION(BlueprintCallable)
	void TickDespawnQueue();
	
	UFUNCTION(BlueprintCallable)
	void ProcessDespawnQueue(TSoftObjectPtr<ABaseAIAnimal>& InAnimal);

	//Release
	UFUNCTION(BlueprintCallable)
	void TryReleaseEntire();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TArray<FAnimalSpawnInfo> AnimalsInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TArray<TSoftObjectPtr<AAnimalSpawnPoint>> SpawnPoints;
	
	//스폰 < 디스폰 < 메모리 해제, //나중에 최적화? 시야범위 관련 사용자 설정에 쓰일 거리 변수들
	//스폰되는 기준 거리 , 스포너 중점으로 부터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	float SpawnDistance = 6000.0f;
	
	//디스폰되는 기준 거리 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	float DespawnDistance = 12000.0f;

	//메모리 해제되는 기준 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	float ReleaseDistance = 14000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") //매 tick 생성될 동물 수
	int32 MaxSpawnPerTick = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") //플레이어와 가까운 지점들 수
	int32 BestSpawnPointsAmount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") //해당 스폰 포인트에 스폰된 동물 수가 이 수치 이하일 경우에만 추가 스폰을 허용
	int32 PermittedToSpawnLimit = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") //매 tick 디스폰될 동물 수
	int32 MaxDespawnPerTick = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning", SaveGame)
	TArray<FAnimalQueueInfo> SaveInfoArray;
	
	TQueue<FAnimalQueueInfo>			  LoadInfoQueue; 
	TQueue<FAnimalQueueInfo>			  CreateInfoQueue;  //매 tick 생성될, 큐에 담길 동물객체 하나마다의 정보를 담는 구조체
	TQueue<TSoftObjectPtr<ABaseAIAnimal>> SpawnQueue;
	TQueue<TSoftObjectPtr<ABaseAIAnimal>> DespawnQueue;
	
	FTimerHandle DistanceTimerHandle;

	bool bIsLoading = false;
};
