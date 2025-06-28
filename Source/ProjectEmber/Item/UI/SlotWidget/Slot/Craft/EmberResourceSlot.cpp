// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberResourceSlot.h"

#include "EmberLog/EmberLog.h"
#include "Item/Core/EmberTmpStruct.h"
#include "Item/Craft/EmberCraftComponent.h"

float UEmberResourceSlot::GetPercentage()
{
	float Percentage = 0.0f;
	
	if (SlotData.Quantity > 0)
	{
		Percentage = FMath::Min((static_cast<float>(CurrentQuantity) / static_cast<float>(SlotData.Quantity)) * 100.f, 100.f);
	}
	 
	return Percentage;
}

void UEmberResourceSlot::UpdateSlot()
{
	if (UEmberCraftComponent* CraftComponent = Cast<UEmberCraftComponent>(DataProvider.GetObject()))
	{

		if (FCraftInfoRow* CraftInfoRow = CraftComponent->CraftItemInfo(SlotIndex))
		{

			if (CraftInfoRow->RequireItems.IsValidIndex(RequireIndex))
			{
				FInstancedStruct InstancedStruct;
				SlotData.InitializeInstancedStruct(InstancedStruct);
				FInstancedStruct CurrentInstancedStruct;
				IEmberResourceProvider::Execute_GetItemInfo(CraftComponent, SlotData, CurrentInstancedStruct);

				if (const FEmberItemEntry* CurrentItemEntry = CurrentInstancedStruct.GetPtr<FEmberItemEntry>())
				{
					CurrentQuantity = CurrentItemEntry->Quantity;

				}
			}
		}
	}
	Super::UpdateSlot();

}

void UEmberResourceSlot::InitCraftComponent(TScriptInterface<IEmberSlotProviderInterface> InDataProvider,
                                            const int32 InSlotIndex, const int32 InRequireIndex)
{
	
	DataProvider = InDataProvider;
	SlotIndex = InSlotIndex;
	RequireIndex = InRequireIndex;

	if (DataProvider)
	{
		if (UEmberCraftComponent* CraftComponent = Cast<UEmberCraftComponent>(DataProvider.GetObject()))
		{
			if (FCraftInfoRow* CraftInfoRow = CraftComponent->CraftItemInfo(SlotIndex))
			{
				if (CraftInfoRow->RequireItems.IsValidIndex(RequireIndex))
				{
					FCraftPair Pair = CraftInfoRow->RequireItems[RequireIndex];
					FInstancedStruct InstancedStruct;
					FEmberItemEntry ItemEntry = FEmberItemEntry(Pair.ItemData.RowName, Pair.Quantity);
					ItemEntry.InitializeInstancedStruct(InstancedStruct);
					SetSlotData(InstancedStruct);

					FInstancedStruct CurrentInstancedStruct;
					IEmberResourceProvider::Execute_GetItemInfo(CraftComponent, ItemEntry, InstancedStruct);

					if (const FEmberItemEntry* CurrentItemEntry = CurrentInstancedStruct.GetPtr<FEmberItemEntry>())
					{
						CurrentQuantity = CurrentItemEntry->Quantity;

					}
				}
			}
		}
	}
}
