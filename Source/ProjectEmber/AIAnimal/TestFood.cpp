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



