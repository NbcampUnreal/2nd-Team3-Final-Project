// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDropComponent.h"

#include "EmberDropItemManager.h"
#include "Character/EmberCharacter.h"
#include "EmberLog/EmberLog.h"
#include "Item/UserItemManger.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UEmberDropComponent::UEmberDropComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	DropManager = CreateDefaultSubobject<UEmberDropItemManager>(TEXT("EmberDropItemManager"));
	if (DropManager)
	{
		DropManager->InitOwner(GetOwner());
		DropManager->InitSlotCount(30, 10);
	}
	// ...
}

void UEmberDropComponent::AddRandomItemToPlayer()
{
	if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UUserItemManger* ItemManger = EmberCharacter->GetItemManager())
		{
			SetRandomItems(EmberCharacter->GetAbilitySystemComponent());
			for (FEmberSlot Slots : *DropManager->GetItemSlotsPtr())
			{
				FInstancedStruct InStruct= Slots.CreateInstancedStruct();
				ItemManger->AddItemAndAlarm(InStruct);
			}
		}
	}
}


void UEmberDropComponent::SetRandomItems(const UAbilitySystemComponent* AbilitySystemComponent)
{
	if (DropManager)
	{
		DropManager->Clear();
		DropManager->SetDropItem(DropID, AbilitySystemComponent);
	}
}

void UEmberDropComponent::ResetItems()
{
	if (DropManager)
	{
		DropManager->Clear();
	}
}

TMap<FName, int32> UEmberDropComponent::GetAllItemInfos_Implementation()
{
	return IEmberResourceProvider::Execute_GetAllItemInfos(DropManager);
}

void UEmberDropComponent::TryConsumeResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems)
{
	IEmberResourceProvider::Execute_TryConsumeResource(DropManager, InRequireItems);
}

void UEmberDropComponent::RemoveResourceUntilAble_Implementation(TArray<FEmberItemEntry>& InRequireItems)
{
	IEmberResourceProvider::Execute_RemoveResourceUntilAble(DropManager, InRequireItems);
}

bool UEmberDropComponent::bConsumeAbleResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems)
{
	return IEmberResourceProvider::Execute_bConsumeAbleResource(DropManager, InRequireItems);
}

