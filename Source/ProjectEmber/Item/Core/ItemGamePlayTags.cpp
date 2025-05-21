// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemGamePlayTags.h"

namespace EmberItemGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Item_Slot_Inventory, "Item.Slot.InventorySlot");
	UE_DEFINE_GAMEPLAY_TAG(Item_Slot_Quick, "Item.Slot.QuickSlot");
	UE_DEFINE_GAMEPLAY_TAG(Item_Slot_Equipment, "Item.Slot.EquipmentSlot");
}

namespace EmberEquipmentGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Head, "Item.Equipment.Head");
	UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Body, "Item.Equipment.Body");
	UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Arm, "Item.Equipment.Arm");
	UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Leg, "Item.Equipment.Leg");
}
