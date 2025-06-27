// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EMSActorSaveInterface.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"
#include "GameFramework/Actor.h"
#include "Interactables/WorldInteractable/WorldInteractableActor.h"
#include "MessageBus/MessageBus.h"
#include "AnimalSpawner.generated.h"

struct FTokenRaidInfo;
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
	int32 FollowCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AloneCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TSoftObjectPtr<ABaseAIAnimal>> SpawnAnimals; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TSoftObjectPtr<ABaseAIAnimal>> DeadAnimals;
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaidClearDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChasingClearDelegate);

UCLASS()
class PROJECTEMBER_API AAnimalSpawner : public AWorldInteractableActor, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	AAnimalSpawner();

	//MiniGame
	UFUNCTION(BlueprintCallable)
	void OnMiniGameEvent();
	//Release
	UFUNCTION(BlueprintCallable) // 전체 메모리해제 , 미니게임 체이싱-> 잡지 못하고 종료됐을 때 호출할 함수
	void TryReleaseEntire();
	
	//Token
	UFUNCTION(BlueprintCallable)
	void OnTokenRaidEvent(FTokenRaidInfo InRow);
	void AddCreateQueueByLocation(TArray<FAnimalSpawnInfo>& InfoByTokenArray);
	void TickCreateQueueByToken(TQueue<FAnimalQueueInfo>& InQueue);
	FVector GetRandomXY(FVector SpawnLocation, FVector Extent);
	FVector SpawnLineTrace(FVector SpawnLocation, float Start, float End);
	UFUNCTION(BlueprintCallable) //파밍 끝나고 죽은 애들만 스폰
	//DespawnQueue 공용으로 사용
	FAnimalInitInfo GetRandomLocationByToken(FVector PlayerLocation);
	UFUNCTION(BlueprintCallable)
	void TryReleaseToken();
	TQueue<FAnimalQueueInfo>			  CreateInfoQueueByToken;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") //전투를 위한 애들
	TArray<FAnimalSpawnInfo> AnimalsInfoByToken;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) //Gameplay.Region.Ocean0
	FGameplayTag IdentityTag;
	UFUNCTION(BlueprintCallable, Category = AI)
	FGameplayTag GetIdentityTag() const;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRaidClearDelegate OnRaidClear;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnChasingClearDelegate OnChasingClear;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;



	//EMS
	virtual void ActorPreSave_Implementation() override;
	virtual void ActorLoaded_Implementation() override;
	
	//메세지버스
	void ReceiveMessage(const FName MessageType, UObject* Payload);
	void MessageMoveToDead(UObject* Payload);
	FMessageDelegate MessageDelegateHandle;

	//Day / Night
	UFUNCTION(BlueprintCallable)
	void OnGameTimeChanged(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
	void MakeRandomActiveAtNight(); //IsDay == true 일 때 확률적으로 D/N 결정하는 함수
	
	//Filter
	UFUNCTION(BlueprintCallable)
	FAnimalInitInfo GetRandomLocationInSpawnVolume(TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint);
	
	UFUNCTION(BlueprintCallable)
	void DistanceCheck();

	UFUNCTION(BlueprintCallable)
	TArray<TSoftObjectPtr<AAnimalSpawnPoint>> DiscardNearestPoint();

	UFUNCTION(BlueprintCallable)
	TArray<TSoftObjectPtr<AAnimalSpawnPoint>> SelectNearPoints(TArray<TSoftObjectPtr<AAnimalSpawnPoint>>& InSpawnPoints);

	
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
	void TrySpawnEntire(TArray<FAnimalSpawnInfo>& InfoArray);

	UFUNCTION(BlueprintCallable) //살아있는 애들만 스폰
	void TrySpawnAlive(TArray<FAnimalSpawnInfo>& InfoArray);
	
	UFUNCTION(BlueprintCallable) //죽은 애들만 스폰
	void TrySpawnDead(TArray<FAnimalSpawnInfo>& InfoArray);

	UFUNCTION(BlueprintCallable)
	void TickSpawnQueue();

	//Despawn
	//디스폰은 동물 마리 단위, 먼 순서대로 스폰되어 있는 모든 동물 정렬, 나중에 최적화? 시야범위 관련 사용자 설정에 쓰일 함수들
	UFUNCTION(BlueprintCallable)
	void SortFarthestAnimal(TArray<FAnimalSpawnInfo>& InfoArray);
	
	UFUNCTION(BlueprintCallable)
	void TickDespawnQueue();
	
	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") //일반적인 애들 
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") //해당 스폰너에서 한 종류의 죽은 동물이 이 수치 이상이면 스폰 허용
	int32 PermittedToSpawnLimit = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning") //매 tick 디스폰될 동물 수
	int32 MaxDespawnPerTick = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning", SaveGame)
	TArray<FAnimalQueueInfo> SaveInfoArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DayNight")
	bool bIsDay = true;
	
	TQueue<FAnimalQueueInfo>			  LoadInfoQueue; 
	TQueue<FAnimalQueueInfo>			  CreateInfoQueue;  //매 tick 생성될, 큐에 담길 동물객체 하나마다의 정보를 담는 구조체
	TQueue<TSoftObjectPtr<ABaseAIAnimal>> SpawnQueue;
	TQueue<TSoftObjectPtr<ABaseAIAnimal>> DespawnQueue;
	
	FTimerHandle DistanceTimerHandle;

	int32 Weather =0;
	bool bIsShouldSleep = true;
	bool bIsLoading = false;
};
