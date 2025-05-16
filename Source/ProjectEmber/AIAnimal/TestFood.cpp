// Fill out your copyright notice in the Description page of Project Settings.


#include "TestFood.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BaseAIAnimal.h"
#include "Abilities/GameplayAbilityTypes.h"


// Sets default values
ATestFood::ATestFood()
{
}

void ATestFood::BeginPlay()
{
	Super::BeginPlay();
}

void ATestFood::Interact_Implementation(AActor* Caller)
{
	IInteractiveObject::Interact_Implementation(Interactor);
	UE_LOG(LogTemp, Warning, TEXT("ATestFood::상호작용 성공, 먹이 삭제함"));

	FGameplayEventData Payload;
	Payload.OptionalObject = this;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Interactor, FGameplayTag::RequestGameplayTag(TEXT("Trigger.Animal.Interact.Harvest")), Payload);
	//Destroy();
}

float ATestFood::GetGatherTime_Implementation()
{
	return Super::GetGatherTime_Implementation();
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
	bIsSelected = InIsSelected;
}



