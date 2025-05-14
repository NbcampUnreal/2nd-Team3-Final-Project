// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberItemStruct.h"
#include "ItemSystemLibrary.h"
#include "ItemTypes.h"
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
			}
		}
	}
}
