// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseDragAbleSlotWidget.h"
#include "EmberBaseSlotWidget.h"
#include "EmberEquipmentSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberEquipmentSlotWidget : public UEmberBaseDragAbleSlotWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlotWidget")
	FGameplayTag EquipmentTypeTag;
};
