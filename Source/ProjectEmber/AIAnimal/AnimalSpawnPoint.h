// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnimalSpawnPoint.generated.h"

struct FAnimalSpawnInfo;

UCLASS()
class PROJECTEMBER_API AAnimalSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	AAnimalSpawnPoint();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool GetIsInHere() const;
	int32 GetAliveAnimalsInBox() const;
	
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
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp,
							 AActor* OtherActor,
							 UPrimitiveComponent* OtherComp,
							 int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	bool bIsInHere = false;
};


