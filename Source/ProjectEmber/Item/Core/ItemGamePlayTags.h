// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace EmberItemGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Inventory);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Quick);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment);
}

namespace EmberEquipmentGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Head);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Body);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Arm);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Leg);
}
