// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIAnimalController.h"
#include "AnimalSpawner.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "TokenRaidInfo.h"
#include "TokenRaidSubsystem.generated.h"

class UEnvQuery;
class ABaseAIAnimal;
/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTEMBER_API UTokenRaidSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// 초기화
	virtual void Initialize(FSubsystemCollectionBase& Collection) override; //테이블 읽어오기

	
	//이벤트 발생했을 때 호출될 시작 함수
	UFUNCTION(BlueprintCallable)
	FTokenRaidInfo GetRaidInfoRow(FGameplayTag Region);
	
	void RegisterWaitingArray(TArray<FAnimalSpawnInfo>& InArray);
	void OnFirstMovementComplete(AActor* InUnit, bool InResult);
	void OnMovementComplete(AActor* InUnit, bool InResult);
	void GiveTokenToRandom();
	void ReturnToken(ABaseAIAnimal* Unit);
	void RunEQSByQuerier();
	UFUNCTION()
	void OnEQSComplete(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	void MovementStart();
	

	FVector GetBestLocation(ABaseAIAnimal& Animal);
	
	void OnUnitDied(ABaseAIAnimal* Unit);
	void RequestToken(ABaseAIAnimal* Unit);
	void SpawnWave(int32 WaveIndex);
	
	// 데이터 테이블 참조
	UPROPERTY(EditDefaultsOnly, Category = "Raid")
	TSoftObjectPtr<UDataTable> TokenRaidDataTable;

	UPROPERTY()
	TArray<FTokenRaidInfo> CurrentRowArray;
	

	UPROPERTY(EditAnywhere)
	int32 MaxActiveTokens = 1;

	UPROPERTY(EditAnywhere)
	int32 CanActiveTokens = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EQS") //실행시킬 eqs
	TSoftObjectPtr<UEnvQuery> EQSQuery;

	TArray<FVector> FoundLocations;
	UPROPERTY()
	TMap<int32,ABaseAIAnimal*> FoundQueriers;
	TMap<ABaseAIAnimal*,FVector> OriLocation;
	TArray<FAnimalSpawnInfo> WaitingArray; //베스트 포지션으로 이동하지 않음
	TArray<TObjectPtr<ABaseAIAnimal>> ReadyUnits; // 베스트 포지션으로 이동 완료함, 토큰 가지지 않은
	TArray<TObjectPtr<ABaseAIAnimal>> ActiveUnits; //토큰 가진, 가졌던
	int32 TotalUnitsPerWave = 0;

};
