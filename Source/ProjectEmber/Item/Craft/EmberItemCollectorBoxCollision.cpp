// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberItemCollectorBoxCollision.h"

#include "EmberCraftComponent.h"
#include "EmberResourceProvider.h"
#include "EmberLog/EmberLog.h"


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
				RequireItems = IEmberResourceProvider::Execute_RemoveResourceUntilAble(ResourceProvider.Get(), RequireItems);

			}

		}
	}
}

bool UEmberItemCollectorBoxCollision::bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems)
{
	TMap<FName, int32> AllItemInfos = GetAllItemInfos_Implementation();
	for (FItemPair RequireItem : InRequireItems)
	{
		int32* Quantity = AllItemInfos.Find(RequireItem.ItemID);
		if (!Quantity || *Quantity < RequireItem.Quantity)
		{
			return false;
		}
	}
	return true;
}

TArray<FItemPair> UEmberItemCollectorBoxCollision::RemoveResourceUntilAble_Implementation(
   const TArray<FItemPair>& InRequireItems)
{
	TArray<FItemPair> RequireItems = InRequireItems;
	for (TWeakObjectPtr<UObject>& ResourceProvider : ResourceProviders)
	{
		if (UObject* Object = ResourceProvider.Get())
		{
			RequireItems = IEmberResourceProvider::Execute_RemoveResourceUntilAble(Object, RequireItems);
			
		}
	}
	return RequireItems;
}
int32 UEmberItemCollectorBoxCollision::DEBUG_GetResourceProviderNum()
{
	return ResourceProviders.Num();
}
