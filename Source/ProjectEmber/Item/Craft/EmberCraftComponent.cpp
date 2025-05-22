// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberCraftComponent.h"

#include "GameFramework/Character.h"
#include "Item/UserItemManger.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UEmberCraftComponent::UEmberCraftComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEmberCraftComponent::InitResourceProvider(const TScriptInterface<IEmberResourceProvider> InResourceProvider)
{
	ResourceProvider = InResourceProvider;
}

FItemPair UEmberCraftComponent::CraftItem(const FName& InItemID)
{
	if (const FCraftInfoRow* CraftInfoRow = CraftDataTable->FindRow<FCraftInfoRow>(InItemID, TEXT("CraftInfo")))
	{
		if (IEmberResourceProvider::Execute_bConsumeAbleResource(ResourceProvider.GetObject(), CraftInfoRow->RequiredItem))
		{
			IEmberResourceProvider::Execute_TryConsumeResource(ResourceProvider.GetObject(), CraftInfoRow->RequiredItem);
			return CraftInfoRow->ResultItem;
		}
	}
	
	return FItemPair();
}


// Called when the game starts
void UEmberCraftComponent::BeginPlay()
{
	Super::BeginPlay();

	if (TObjectPtr<UUserItemManger> ItemManager = GetWorld()->GetFirstPlayerController()->GetCharacter()->GetComponentByClass<UUserItemManger>())
	{
		InitResourceProvider(ItemManager);
	}
	// ...
	
}

