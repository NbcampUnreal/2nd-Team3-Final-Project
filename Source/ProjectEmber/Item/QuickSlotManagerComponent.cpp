#include "QuickSlotManagerComponent.h"

UQuickSlotManagerComponent::UQuickSlotManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	InventoryCapacity = QuickSlotCapacity;
}

void UQuickSlotManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}


