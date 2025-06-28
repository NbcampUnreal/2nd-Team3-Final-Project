// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberResourceWidget.h"

void UEmberResourceWidget::InitCraft(UEmberCraftComponent* InCraftComponent, const TArray<FCraftPair>& InRequestInfos)
{
	CraftComponent = InCraftComponent;
	RequestInfos = InRequestInfos;
}
