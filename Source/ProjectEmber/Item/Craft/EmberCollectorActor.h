// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmberCollectorActor.generated.h"

class UEmberItemCollectorBoxCollision;
class UEmberItemCollectorComponent;

UCLASS()
class PROJECTEMBER_API AEmberCollectorActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEmberCollectorActor();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	TObjectPtr<UEmberItemCollectorBoxCollision> CollectorCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	TObjectPtr<USceneComponent> SceneComponent;
};
