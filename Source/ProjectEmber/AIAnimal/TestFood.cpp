// Fill out your copyright notice in the Description page of Project Settings.


#include "TestFood.h"

#include "AbilitySystemComponent.h"


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
	Super::Interact_Implementation(Caller);
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



