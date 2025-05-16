// Fill out your copyright notice in the Description page of Project Settings.


#include "UserItemManger.h"

#include "InventoryManagerComponent.h"
#include "QuickSlotManagerComponent.h"


// Sets default values for this component's properties
UUserItemManger::UUserItemManger()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	InventoryManager = CreateDefaultSubobject<UInventoryManagerComponent>(TEXT("InventoryManager"));
	QuickSlotManager = CreateDefaultSubobject<UQuickSlotManagerComponent>(TEXT("QuickSlotManager"));
	// ...
}


// Called when the game starts
void UUserItemManger::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UUserItemManger::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FEmberItemInfo UUserItemManger::GetQuickSlotInfo(int32 InIndex)
{
	return FEmberItemInfo(QuickSlotManager->GetSlotDataByIndex(InIndex));
}

void UUserItemManger::UseQuickSlotInfo(int32 InIndex)
{
	QuickSlotManager->UseItemInSlot_Implementation(InIndex);
}

FEmberItemInfo UUserItemManger::GetInventorySlotInfo(int32 InIndex)
{
	return FEmberItemInfo(InventoryManager->GetSlotDataByIndex(InIndex));
}

void UUserItemManger::UseInventorySlotInfo(int32 InIndex)
{
	InventoryManager->UseItemInSlot_Implementation(InIndex);
}

void UUserItemManger::AddItem(FName ItemID, int32 Quantity)
{
	InventoryManager->AddItemAndHandleOverflow_Implementation(ItemID, Quantity, GetOwner()->GetActorLocation(), FRotator::ZeroRotator);
}

const class UInventoryManagerComponent* UUserItemManger::GetInventoryManager() const
{
	return InventoryManager;
}

UInventoryManagerComponent* UUserItemManger::GetInventoryManager()
{
	return InventoryManager;
}

const class UQuickSlotManagerComponent* UUserItemManger::GetQuickSlotManagerComponent() const
{
	return QuickSlotManager;
}

UQuickSlotManagerComponent* UUserItemManger::GetQuickSlotManagerComponent()
{
	return QuickSlotManager;
}

