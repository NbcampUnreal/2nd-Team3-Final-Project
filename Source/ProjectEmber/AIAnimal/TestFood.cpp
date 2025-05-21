// Fill out your copyright notice in the Description page of Project Settings.


#include "TestFood.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BaseAIAnimal.h"
#include "Abilities/GameplayAbilityTypes.h"


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

void ATestFood::Interact_Implementation(AActor* Caller)
{
	Super::Interact_Implementation(Caller);

	UE_LOG(LogTemp, Warning, TEXT("ATestFood::상호작용 성공, 먹이 삭제함"));

	FGameplayEventData Payload;
	Payload.OptionalObject = this;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Caller, FGameplayTag::RequestGameplayTag(TEXT("Trigger.Animal.Interact.Harvest")), Payload);
	//Destroy();
}

float ATestFood::GetGatherTime_Implementation()
{
	return Super::GetGatherTime_Implementation();
}

void ATestFood::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTagContainer;
}

bool ATestFood::GetSelected()
{
	return bIsSelected;
}

const FGameplayTagContainer& ATestFood::GetGameplayTagContainer()
{
	return GameplayTagContainer;
}

void ATestFood::SetSelected(const bool InIsSelected)
{
}
