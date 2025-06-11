// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropComponent.h"

#include "EmberDropItemManager.h"
#include "EmberLog/EmberLog.h"


// Sets default values for this component's properties
UEmberDropComponent::UEmberDropComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	DropManager = CreateDefaultSubobject<UEmberDropItemManager>(TEXT("EmberDropItemManager"));
	if (DropManager)
	{
		DropManager->InitSlot(30, 10, GetOwner());
	}
	// ...
}


void UEmberDropComponent::SetRandomItems(const UAbilitySystemComponent* AbilitySystemComponent)
{
	if (DropManager)
	{
		DropManager->SetDropItem(DropID, AbilitySystemComponent);
	}
}

TMap<FName, int32> UEmberDropComponent::GetAllItemInfos_Implementation()
{
	return IEmberResourceProvider::Execute_GetAllItemInfos(DropManager);
}

void UEmberDropComponent::TryConsumeResource_Implementation(const TArray<FItemPair>& InRequireItems)
{
	IEmberResourceProvider::Execute_TryConsumeResource(DropManager, InRequireItems);
}

void UEmberDropComponent::RemoveResourceUntilAble_Implementation(TArray<FItemPair>& InRequireItems)
{
	IEmberResourceProvider::Execute_RemoveResourceUntilAble(DropManager, InRequireItems);
}

bool UEmberDropComponent::bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems)
{
	return IEmberResourceProvider::Execute_bConsumeAbleResource(DropManager, InRequireItems);
}

