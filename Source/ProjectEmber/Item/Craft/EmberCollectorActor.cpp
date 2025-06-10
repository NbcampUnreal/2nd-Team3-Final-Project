// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCollectorActor.h"

#include "EmberItemCollectorBoxCollision.h"
#include "EmberItemCollectorComponent.h"


// Sets default values
AEmberCollectorActor::AEmberCollectorActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);
	CollectorCollision = CreateDefaultSubobject<UEmberItemCollectorBoxCollision>(TEXT("ItemCollectorBoxComponent"));
	CollectorCollision->SetupAttachment(GetRootComponent());
}


