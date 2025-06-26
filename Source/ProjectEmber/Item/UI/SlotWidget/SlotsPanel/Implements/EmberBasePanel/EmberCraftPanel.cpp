// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftPanel.h"

#include "EmberLog/EmberLog.h"
#include "Item/Craft/EmberCraftComponent.h"
#include "Item/ItemContainer/Implements/EmberBaseResourceSlotContainer/EmberCraftContainer.h"

void UEmberCraftPanel::InitData(UEmberCraftComponent* InCraftComponent)
{
	CraftComponent = InCraftComponent;

	if (CraftComponent)
	{
		CraftSelectInfos.Reset();
		for (FCraftInfoRow* CraftInfoRow : CraftComponent->CraftItemInfos())
		{
			if (CraftInfoRow)
			{
				FCraftSelectInfo CraftSelectInfo = FCraftSelectInfo(*CraftInfoRow);
				CraftSelectInfos.Add(CraftSelectInfo);
			}
		}
	}
}

void UEmberCraftPanel::StartCraftingItem(int32 CraftingIndex)
{
	if (CraftComponent && CraftSelectInfos.IsValidIndex(CraftingIndex))
	{
		CraftComponent->CreateCraftItem(CraftSelectInfos[CraftingIndex].ResultItemData.ItemID);
	}
}

void UEmberCraftPanel::BindToManagerDelegates_Implementation()
{
	UnBindToManagerDelegates();

	EMBER_LOG(LogTemp, Warning, TEXT("ABCD"));
	if (DataProvider && DataProvider.GetObject())
	{
		EMBER_LOG(LogTemp, Warning, TEXT("ABCD1"));

		if (TObjectPtr<UEmberCraftComponent> EmberCraftComponent = Cast<UEmberCraftComponent>(DataProvider.GetObject()))
		{
			EMBER_LOG(LogTemp, Warning, TEXT("ABCD2"));

			if (TObjectPtr<UEmberCraftContainer> EmberCraftContainer = EmberCraftComponent->GetSlotContainer())
			{
				EMBER_LOG(LogTemp, Warning, TEXT("ABCD3"));

				EmberCraftContainer->OnItemChangedDelegate.AddDynamic(this, &UEmberCraftPanel::SlotChanged);
			}
		}
	}
}

void UEmberCraftPanel::UnBindToManagerDelegates_Implementation()
{
	if (DataProvider && DataProvider.GetObject())
	{
		if (TObjectPtr<UEmberCraftComponent> EmberCraftComponent = Cast<UEmberCraftComponent>(DataProvider.GetObject()))
		{
			if (TObjectPtr<UEmberCraftContainer> EmberCraftContainer = EmberCraftComponent->GetSlotContainer())
			{
				EmberCraftContainer->OnItemChangedDelegate.RemoveDynamic(this, &UEmberCraftPanel::SlotChanged);
			}
		}
	}
}

