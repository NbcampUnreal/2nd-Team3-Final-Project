// Fill out your copyright notice in the Description page of Project Settings.


#include "TestFood.h"

#include "BaseAIAnimal.h"


// Sets default values
ATestFood::ATestFood()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATestFood::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestFood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestFood::Interact_Implementation(AActor* Interactor)
{
	IInteractiveObject::Interact_Implementation(Interactor);
	UE_LOG(LogTemp, Warning, TEXT("ATestFood::상호작용 성공, 먹이 삭제함"));
	Destroy();
}

FGameplayTag ATestFood::GetTag_Implementation()
{
	IInteractiveObject::GetTag_Implementation();
	return GameplayTag;
}

