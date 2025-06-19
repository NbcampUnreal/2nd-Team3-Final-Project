// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionFragment_Spawn.h"
#include "InteractionFragment_Spawn_Spline.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInteractionFragment_Spawn_Spline : public UInteractionFragment_Spawn
{
	GENERATED_BODY()
public:
	UInteractionFragment_Spawn_Spline();

	virtual void OnRegister() override;
	
	virtual void SpawnActors(UWorld* World) override;

	virtual void DestroySpawnedActors() override;

protected:
	UPROPERTY()
	TObjectPtr<USplineComponent> TargetSpline; // Owner의 스플라인 참조

	UPROPERTY()
	TArray<AActor*> SpawnedList; //실제 스폰된 객체들
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TArray<TSubclassOf<AActor>> SpawnList; // 스폰할 클래스 타입들(스플라인 포인트가 더 많다면 배열을 순환)

	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AActor> DefaultActorClass; // SpawnList의 요소가 null일 때 대신 스폰될 클래스
};
