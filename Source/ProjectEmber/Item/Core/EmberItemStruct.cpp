// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberItemStruct.h"
#include "ItemSystemLibrary.h"
#include "ItemTypes.h"
#include "EmberLog/EmberLog.h"
#include "Item/ItemSubsystem.h"

FEmberSlotData::FEmberSlotData(const FName& InItemID, const int32 InQuantity, const TArray<FItemEffectApplicationInfo>& InEnchantEffects)
{
	if (UItemSystemLibrary::GetItemSubsystem())
	{
		if (const FItemMasterInfoRow* InItemMasterInfo = UItemSystemLibrary::GetItemSubsystem()->GetItemMasterInfoRow(InItemID))
		{
			ItemID = InItemID;
			Quantity = InQuantity;
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
	EnchantEffects = InEnchantEffects;
}

FEquipmentSlotData::FEquipmentSlotData(const FEmberSlotData& InEmberSlot) : FEmberSlotData(InEmberSlot)
{
	if (InEmberSlot.EquipmentData)
	{
		if (FEquipmentInfoRow* Row = InEmberSlot.EquipmentData->GetRow<FEquipmentInfoRow>(TEXT("EquipmentInfo")))
		{
			EquipmentInfo = FEquipmentInfoRow(*Row);
		}
	}
}


FEquipmentSlotData::FEquipmentSlotData(const FName& InItemID, const int32 InQuantity) : FEmberSlotData(InItemID, InQuantity)
{
	if (EquipmentData)
	{
		if (FEquipmentInfoRow* Row = EquipmentData->GetRow<FEquipmentInfoRow>(TEXT("EquipmentInfo")))
		{
			EquipmentInfo = FEquipmentInfoRow(*Row);
		}
	}
}

FEmberItemInfo::FEmberItemInfo(const FEmberSlotData& InItemInventorySlotData)
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
			EMBER_LOG(LogTemp, Warning, TEXT("Item : %d"), EquipmentInfo->MainEffects.Num());
			for (FDataTableRowHandle& Handle : EquipmentInfo->MainEffects)
			{
				if (!Handle.DataTable ||Handle.IsNull())
				{
					InActiveEffects.Add(*Handle.GetRow<FItemEffectApplicationInfo>(TEXT("EquipmentInfo")));
				}
			}
			ItemTags.AddTag(EquipmentInfo->EquipmentTag);
		}
	}
	ActiveEffects = InActiveEffects;
}
