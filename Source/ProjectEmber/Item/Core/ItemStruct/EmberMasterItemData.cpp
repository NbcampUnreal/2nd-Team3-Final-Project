// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberMasterItemData.h"

#include "Item/ItemSubsystem.h"
#include "Item/Core/ItemSystemLibrary.h"

FEmberMasterItemData::FEmberMasterItemData(const FName& InItemID, const int32 InQuantity,
                                           const TArray<FItemEffectApplicationInfo>& InEnchantEffects) : FEmberItemEntry(InItemID, InQuantity, InEnchantEffects)
{
	CashedMasterItemData();
}

FEmberMasterItemData::FEmberMasterItemData(const FEmberItemEntry& EmberItemEntry)
{
	*this = FEmberMasterItemData(EmberItemEntry.ItemID, EmberItemEntry.Quantity, EmberItemEntry.Enchants);
}

FEmberMasterItemData::FEmberMasterItemData(const FInstancedStruct& InInstancedStruct) : FEmberItemEntry(InInstancedStruct)
{
	if (const FEmberMasterItemData* InItemMasterInfo = InInstancedStruct.GetPtr<FEmberMasterItemData>())
	{
		*this = *InItemMasterInfo;
	}
	else
	{
		CashedMasterItemData();
	}
}

void FEmberMasterItemData::CashedMasterItemData()
{
	if (UItemSystemLibrary::GetItemSubsystem())
	{
		if (const FItemMasterInfoRow* InItemMasterInfo = UItemSystemLibrary::GetItemSubsystem()->GetItemMasterInfoRow(ItemID))
		{
			ItemDisplayName = InItemMasterInfo->ItemDisplayName;
			ItemDescription = InItemMasterInfo->ItemDescription;
			
			for (const FDataTableRowHandle& Handle : InItemMasterInfo->ItemData)
			{
				if (Handle.IsNull() || !Handle.DataTable) continue;

				if (const UScriptStruct* RowStructType = Handle.DataTable->GetRowStruct())
				{
					if (const uint8* RowDataPtr = Handle.DataTable->FindRowUnchecked(Handle.RowName))
					{
						FInstancedStruct OutInstancedStruct = FInstancedStruct();
						OutInstancedStruct.InitializeAs(RowStructType, RowDataPtr);
						DataInstancedStructs.Add(MoveTemp(OutInstancedStruct));
					}
				}
			}
		}
	}
}
