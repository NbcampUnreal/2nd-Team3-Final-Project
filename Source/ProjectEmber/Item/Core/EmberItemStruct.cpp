// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberItemStruct.h"
#include "ItemSystemLibrary.h"
#include "ItemTypes.h"
#include "EmberLog/EmberLog.h"
#include "Item/ItemSubsystem.h"

FInventorySlotData::FInventorySlotData(const FName& InItemID)
{
	if (const FItemMasterInfoRow* InItemMasterInfo = UItemSystemLibrary::GetItemSubsystem()->GetItemMasterInfoRow(InItemID))
	{
		ItemID = InItemID;
		ItemDisplayName = InItemMasterInfo->ItemDescription;
		ItemDescription = InItemMasterInfo->ItemDescription;

		for (const FDataTableRowHandle& Handle : InItemMasterInfo->ItemData)
		{
			if (Handle.IsNull() || !Handle.DataTable) continue;


			if (const UScriptStruct* RowStructType = Handle.DataTable->GetRowStruct())
			{
				if (RowStructType == FSlotInfoRow::StaticStruct())
				{
					SlotData = Handle;
					if (const FSlotInfoRow* InvData = Handle.GetRow<FSlotInfoRow>(TEXT("FInventorySlotData_Constructor_Inv")))
					{
						MaxStackSize = (InvData->MaxStackSize > 0) ? InvData->MaxStackSize : 1;
					}
				}
				else if (RowStructType == FConsumableInfoRow::StaticStruct())
				{
					ConsumableData = Handle;
				}
				else if (RowStructType == FEquipmentInfoRow::StaticStruct())
				{
					EquipmentData = Handle;
				}
			}
		}
	}
}

FQuickSlotData::FQuickSlotData(int32 InInventorySlotIndex, const FInventorySlotData& InInventorySlotData)
{
	InventorySlotIndex = InInventorySlotIndex;
	InventorySlotData = FInventorySlotData(InInventorySlotData);
}

FEmberItemInfo::FEmberItemInfo(const FInventorySlotData& InItemInventorySlotData)
{
	ItemID = InItemInventorySlotData.ItemID;

	TArray<FItemEffectApplicationInfo> InActiveEffects;

	if (InItemInventorySlotData.ConsumableData.IsSet())
	{
		if (FConsumableInfoRow* ConsumeInfo = InItemInventorySlotData.ConsumableData->GetRow<FConsumableInfoRow>(TEXT("ConsumeInfo")))
		{
			InActiveEffects.Append(ConsumeInfo->EffectsToApplyOnConsume);
		}
	}
	if (InItemInventorySlotData.EquipmentData.IsSet())
	{
		if (FEquipmentInfoRow* EquipmentInfo = InItemInventorySlotData.EquipmentData->GetRow<FEquipmentInfoRow>(TEXT("ConsumeInfo")))
		{
			EMBER_LOG(LogTemp, Warning, TEXT("Item : %d"), EquipmentInfo->ActiveEffects.Num());
			InActiveEffects.Append(EquipmentInfo->ActiveEffects);
			ItemTags.AddTag(EquipmentInfo->EquipmentTag);
		}
	}
	ActiveEffects = InActiveEffects;
}
