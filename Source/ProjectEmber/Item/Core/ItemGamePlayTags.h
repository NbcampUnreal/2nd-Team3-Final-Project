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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Helmet);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Necklace);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Chest);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Legs);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Boots);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Weapon);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Cape);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Bracelet);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Gloves);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Belt);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Ring);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Shield);
}

namespace EmberItemEffectGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Effect_MaxHealth);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Effect_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Effect_DropRate);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Effect_DropBonus);
}
