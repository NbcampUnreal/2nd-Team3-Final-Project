// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Fragments/InteractionFragment_Spawn.h"
#include "InteractionFragment_Spawn_Points.generated.h"

UCLASS()
class ASpawnPoint : public AActor
{
	GENERATED_BODY()
public:
	ASpawnPoint()
	{
		Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
		RootComponent = Root;
	}
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	FVector PreviewOffset = FVector::ZeroVector;

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override
	{
		DrawDebugSphere(GetWorld(), GetActorLocation() + PreviewOffset, 20, 20, FColor::Green);
	}
#endif
};



//UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
UCLASS() // 아직 미구현 상태라 빼둠
class PROJECTEMBER_API UInteractionFragment_Spawn_Points : public UInteractionFragment_Spawn
{
	GENERATED_BODY()
public:
	UInteractionFragment_Spawn_Points();
	
	virtual void OnRegister() override;
	
	virtual void SpawnActors(UWorld* World) override;

	virtual void DestroySpawnedActors() override;

protected:
	//스폰 위치랑 클래스를 정할 수 있는 클래스나 구조체 배열
	//에디터에서 동적으로 생성이 가능해야함.
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TArray<TObjectPtr<ASpawnPoint>> SpawnPoints;
	
	UPROPERTY()
	TArray<AActor*> SpawnedList; //실제 스폰된 객체들
};
