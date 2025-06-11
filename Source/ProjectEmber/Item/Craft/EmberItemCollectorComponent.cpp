// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberItemCollectorComponent.h"

#include "EmberCraftComponent.h"
#include "Components/BoxComponent.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
UEmberItemCollectorComponent::UEmberItemCollectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CollectRangeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollectRangeBox"));
	if (CollectRangeBox && GetOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			
			/*
			CollectRangeBox->SetupAttachment(Character->GetMesh());
			*/

		}
	}
	// ...
}


// Called when the game starts
void UEmberItemCollectorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (CollectRangeBox)
	{/*
		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			CollectRangeBox->SetupAttachment(Character->GetMesh());
			CollectRangeBox->SetWorldLocation(Character->GetActorLocation());
			CollectRangeBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			CollectRangeBox->SetHiddenInGame(true);
		}*/
	}
	FindOverlappingResourceComponent();
}

void UEmberItemCollectorComponent::FindOverlappingResourceComponent()
{
	if (!CollectRangeBox)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;

	CollectRangeBox->GetOverlappingActors(OverlappingActors);

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

TArray<TWeakObjectPtr<UObject>> UEmberItemCollectorComponent::GetResourceProvider()
{
	return ResourceProviders;
}

void UEmberItemCollectorComponent::SetResourceProvider(TScriptInterface<UEmberResourceProvider> Provider)
{
	if (Provider.GetObject())
	{
		ResourceProviders.Add(MakeWeakObjectPtr(Provider.GetObject()));

	}

}

TMap<FName, int32> UEmberItemCollectorComponent::GetAllItemInfos_Implementation()
{
	TMap<FName, int32> AllItemInfos;
	
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

void UEmberItemCollectorComponent::TryConsumeResource_Implementation(const TArray<FItemPair>& InRequireItems)
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

bool UEmberItemCollectorComponent::bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems)
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

void UEmberItemCollectorComponent::RemoveResourceUntilAble_Implementation(
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
int32 UEmberItemCollectorComponent::DEBUG_GetResourceProviderNum()
{
	return ResourceProviders.Num();
}
