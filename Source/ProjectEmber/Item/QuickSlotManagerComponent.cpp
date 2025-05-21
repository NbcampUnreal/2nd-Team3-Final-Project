#include "QuickSlotManagerComponent.h"

UQuickSlotManagerComponent::UQuickSlotManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuickSlotManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

FName UQuickSlotManagerComponent::SelectQuickSlot(const int32 InIndex)
{
	if (CurrentQuickSlotIndex == InIndex)
	{
		CurrentQuickSlotIndex = -1;
		return FName();
	}
	else
	{
		CurrentQuickSlotIndex = InIndex;
		return GetSlotItemID_Implementation(CurrentQuickSlotIndex);
	}
}


