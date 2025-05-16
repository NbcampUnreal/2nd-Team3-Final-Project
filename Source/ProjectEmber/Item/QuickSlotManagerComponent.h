// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManagerComponent.h"
#include "Components/ActorComponent.h"
#include "Core/EmberItemStruct.h"
#include "UI/SlotWidget/EmberSlotDataProviderInterface.h"
#include "QuickSlotManagerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuickSlotChangedDelegate, int32, SlotIndex, const FQuickSlotData&, SlotData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UQuickSlotManagerComponent : public UInventoryManagerComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UQuickSlotManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	
};
