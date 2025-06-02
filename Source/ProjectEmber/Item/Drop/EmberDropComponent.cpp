// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropComponent.h"

#include "EmberDropItemManager.h"


// Sets default values for this component's properties
UEmberDropComponent::UEmberDropComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	DropManager = CreateDefaultSubobject<UEmberDropItemManager>(TEXT("EmberDropItemManager"));

	// ...
}


void UEmberDropComponent::SetRandomItems()
{
	if (DropManager)
	{
		DropManager->SetDropItem(DropID);
	}
}

