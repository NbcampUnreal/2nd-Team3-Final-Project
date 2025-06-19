// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberTmpStruct.h"
#include "ItemTypes.h"
#include "ItemSystemLibrary.h"
#include "Item/ItemSubsystem.h"


void FEmberItemEntry::AddQuantity(const int32 InQuantity)
{
	Quantity += InQuantity;
}

void FEmberItemEntry::RemoveQuantity(const int32 InQuantity)
{
	Quantity -= InQuantity;
}

FEmberItemKey FEmberItemEntry::CreateItemKey() const
{
	return FEmberItemKey(ItemID, Enchants);
}

FEmberMasterItemData::FEmberMasterItemData(const FName& InItemID, const int32 InQuantity,
	const TArray<FItemEffectApplicationInfo>& InEnchantEffects)
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
	EnchantEffects = InEnchantEffects;
}

FEmberItemKey FEmberMasterItemData::CreateItemKey() const
{
	return FEmberItemKey(ItemID, EnchantEffects);
}

void FEmberMasterItemData::RemoveQuantity(int32& InQuantity)
{
	int32 CurrentQuantity = FMath::Min(Quantity, InQuantity);
	Quantity -= CurrentQuantity;
	if (Quantity <= 0)
	{
		Clear();
	}
}

void FEmberMasterItemData::AddQuantity(int32& InQuantity)
{
	int32 CurrentQuantity = FMath::Min(ITEM_SYSTEM_MAX_STACK - Quantity, InQuantity);
	Quantity += CurrentQuantity;
}

void FEmberTotalSlot::AddIndex(int32 InIndex)
{
	ItemIndexes.Add(InIndex);
}

void FEmberTotalSlot::RemoveIndex(int32 InIndex)
{
	ItemIndexes.Remove(InIndex);
}
