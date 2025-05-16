// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/DragDropOperation.h"
#include "Item/UI/SlotWidget/EmberSlotDataProviderInterface.h"

#include "EmberItemSlotDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberItemSlotDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Payload", meta=(ExposeOnSpawn=true))
	int32 SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Payload", meta=(ExposeOnSpawn=true))
	int32 DraggedQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Payload", meta=(ExposeOnSpawn=true))
	TScriptInterface<UEmberSlotDataProviderInterface> Provider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category="Payload", meta=(ExposeOnSpawn=true))
	FGameplayTag SlotType; 
	
};
