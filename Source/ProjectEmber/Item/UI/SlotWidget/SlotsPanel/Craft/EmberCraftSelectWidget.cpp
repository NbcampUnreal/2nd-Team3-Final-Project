// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftSelectWidget.h"

#include "EmberLog/EmberLog.h"
#include "Item/Core/ItemCraftType.h"
#include "Item/Craft/EmberCraftComponent.h"

void UEmberCraftSelectWidget::InitData(UEmberCraftComponent* InCraftComponent)
{
	CraftComponent = InCraftComponent;

	if (CraftComponent)
	{
		for (FCraftInfoRow& CraftInfoRow : CraftComponent->CraftItemInfos())
		{
			CraftSelectInfos.Add(FCraftSelectInfo(CraftInfoRow));
		}
	}
}

