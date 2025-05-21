#include "QuickSlotManagerComponent.h"

UQuickSlotManagerComponent::UQuickSlotManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuickSlotManagerComponent::BeginPlay()
{
	InventoryCapacity = QuickSlotCapacity;
	Super::BeginPlay();
}


