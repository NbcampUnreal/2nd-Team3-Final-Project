// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberItemCollectorBoxCollision.h"

#include "EmberCraftComponent.h"
#include "EmberResourceProvider.h"
#include "EmberLog/EmberLog.h"
#include "Item/Core/EmberTmpStruct.h"


// Sets default values for this component's properties
UEmberItemCollectorBoxCollision::UEmberItemCollectorBoxCollision()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEmberItemCollectorBoxCollision::PostInitProperties()
{
	Super::PostInitProperties();
	
	
	this->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->SetGenerateOverlapEvents(true);
}


// Called when the game starts
void UEmberItemCollectorBoxCollision::BeginPlay()
{
	Super::BeginPlay();
	// ...
	FindOverlappingResourceComponent();

}


void UEmberItemCollectorBoxCollision::FindOverlappingResourceComponent()
{

	TArray<AActor*> OverlappingActors;

	this->GetOverlappingActors(OverlappingActors);

	ResourceProviders.Empty();

	for (TObjectPtr<AActor> Actor : OverlappingActors)
	{

		if (Actor && Actor != GetOwner())
		{

			for (TObjectPtr<UActorComponent> Component : Actor->GetComponentsByInterface(UEmberResourceProvider::StaticClass()))
			{

				if (!Component.IsA(UEmberCraftComponent::StaticClass()))
				ResourceProviders.Add(Component);
			}
		}
	}
}

TArray<TWeakObjectPtr<UObject>> UEmberItemCollectorBoxCollision::GetResourceProvider()
{
	return ResourceProviders;
}

void UEmberItemCollectorBoxCollision::SetResourceProvider(TScriptInterface<UEmberResourceProvider> Provider)
{
	if (Provider.GetObject())
	{
		ResourceProviders.Add(MakeWeakObjectPtr(Provider.GetObject()));

	}

}

TMap<FName, int32> UEmberItemCollectorBoxCollision::GetAllItemInfos_Implementation()
{
	TMap<FName, int32> AllItemInfos;
	FindOverlappingResourceComponent();

	for (TWeakObjectPtr<UObject>& ResourceProvider : ResourceProviders)
	{
		if (ResourceProvider.Get())
		{
			TMap<FName, int32> ProviderItems = IEmberResourceProvider::Execute_GetAllItemInfos(ResourceProvider.Get());
			for (const auto& Item : ProviderItems)
			{
				if (int32* ItemPtr = AllItemInfos.Find(Item.Key))
				{
					*ItemPtr += Item.Value;
				}
				else
				{
					AllItemInfos.Emplace(Item.Key, Item.Value);
				}
			}
		}
	}
	return AllItemInfos;
}

void UEmberItemCollectorBoxCollision::TryConsumeResource_Implementation(const TArray<FItemPair>& InRequireItems)
{
	TArray<FItemPair> RequireItems = InRequireItems;

	if (bConsumeAbleResource_Implementation(RequireItems))
	{
		for (TWeakObjectPtr<UObject>& ResourceProvider : ResourceProviders)
		{
			if (ResourceProvider.Get())
			{
				IEmberResourceProvider::Execute_RemoveResourceUntilAble(ResourceProvider.Get(), RequireItems);
			}

		}
	}
}

bool UEmberItemCollectorBoxCollision::bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems)
{
	TArray<FEmberItemEntry> RequiresEntries;
	TMap<FEmberItemKey, FInstancedStruct> OutItemInfos;
	for (FItemPair RequireItem : InRequireItems)
	{
		RequiresEntries.Add(FEmberItemEntry(RequireItem.ItemID, RequireItem.Quantity, RequireItem.Enchants));
	}
	
	for (auto& ResourceProviderPtr : ResourceProviders)
	{
		if (UObject* ResourceProvider = ResourceProviderPtr.Get())
		{
			IEmberResourceProvider::Execute_GetItemInfos(ResourceProvider, RequiresEntries, OutItemInfos);
		}
	}

	for (auto& RequiresEntry : RequiresEntries)
	{
		if (FInstancedStruct* OutItemInstanced = OutItemInfos.Find(RequiresEntry.CreateItemKey()))
		{
			if (const FEmberMasterItemData* Data = OutItemInstanced->GetPtr<FEmberMasterItemData>())
			{
				if (Data->Quantity >= RequiresEntry.Quantity)
				{
					continue;
				}
			}
		}
		return false;
	}
	return true;
}

void UEmberItemCollectorBoxCollision::GetItemInfo_Implementation(FEmberItemEntry& InItemEntry,
	FInstancedStruct& OutItemInfo)
{
	for (TWeakObjectPtr<UObject>& ResourceProviderPtr : ResourceProviders)
	{
		if (UObject* ResourceProvider = ResourceProviderPtr.Get())
		{
			IEmberResourceProvider::Execute_GetItemInfo(ResourceProvider, InItemEntry, OutItemInfo);
		}
	}
}

void UEmberItemCollectorBoxCollision::GetItemInfos_Implementation(TArray<FEmberItemEntry>& InItemEntries,
	TMap<FEmberItemKey, FInstancedStruct>& OutItemInfos)
{
	for (TWeakObjectPtr<UObject>& ResourceProviderPtr : ResourceProviders)
	{
		if (UObject* ResourceProvider = ResourceProviderPtr.Get())
		{
			IEmberResourceProvider::Execute_GetItemInfos(ResourceProvider, InItemEntries, OutItemInfos);
		}
	}
}

void UEmberItemCollectorBoxCollision::RemoveResourceUntilAble_Implementation(
   TArray<FItemPair>& InRequireItems)
{
	for (TWeakObjectPtr<UObject>& ResourceProvider : ResourceProviders)
	{
		if (UObject* Object = ResourceProvider.Get())
		{
			IEmberResourceProvider::Execute_RemoveResourceUntilAble(Object, InRequireItems);
		}
	}
}

int32 UEmberItemCollectorBoxCollision::DEBUG_GetResourceProviderNum()
{
	return ResourceProviders.Num();
}
