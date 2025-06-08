// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftComponent.h"

#include "EmberLog/EmberLog.h"
#include "Item/UserItemManger.h"
#include "Item/Core/ItemCraftType.h"


// Sets default values for this component's properties
UEmberCraftComponent::UEmberCraftComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

FItemPair UEmberCraftComponent::CraftItem(const FName& InItemID)
{
	FItemPair ReturnItem;
	EMBER_LOG(LogEmberItem, Warning, TEXT("abcd"));

	if (const FCraftInfoRow* CraftInfoRow = CraftDataTable->FindRow<FCraftInfoRow>(InItemID, TEXT("CraftInfo")))
	{
		TArray<FItemPair> RequireItems;
		FItemPair RequestItem;
		EMBER_LOG(LogEmberItem, Warning, TEXT("abcd1"));

		if (!CraftInfoRow->RequestItem.ItemData.IsNull())
		{
			EMBER_LOG(LogEmberItem, Warning, TEXT("abcd2"));

			RequestItem = FItemPair(CraftInfoRow->RequestItem.ItemData.RowName, CraftInfoRow->RequestItem.Quantity);
		}
		for (const FCraftPair& RequireItem : CraftInfoRow->RequireItems)
		{
			if (!RequireItem.ItemData.IsNull())
			{
				EMBER_LOG(LogEmberItem, Warning, TEXT("abcd3"));

				RequireItems.Add(FItemPair(RequireItem.ItemData.RowName, RequireItem.Quantity));
			}
		}
		for (auto& a : IEmberResourceProvider::Execute_GetAllItemInfos(this))
		{
			EMBER_LOG(LogEmberItem, Warning, TEXT("abcd4 %s : %d"), *a.Key.ToString(), a.Value);
		}

		if (IEmberResourceProvider::Execute_bConsumeAbleResource(this, RequireItems))
		{
			IEmberResourceProvider::Execute_TryConsumeResource(this, RequireItems);
			
			ReturnItem = RequestItem;
		}
	}
	
	return ReturnItem;
}

