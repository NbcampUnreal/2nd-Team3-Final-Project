// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberInteractableItemDropComponent.h"

#include "Character/EmberCharacter.h"
#include "Item/ItemSubsystem.h"
#include "Item/Core/ItemSystemLibrary.h"
#include "Item/UserItemManger.h"


// Sets default values for this component's properties
UEmberInteractableItemDropComponent::UEmberInteractableItemDropComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEmberInteractableItemDropComponent::DropItem(AActor* Actor)
{
	if (TObjectPtr<AEmberCharacter> Character = Cast<AEmberCharacter>(Actor))
	{
		if (UUserItemManger* ItemManager = Character->GetItemManager())
		{
			for (const auto& Item : GetDropItemName(ItemName, Character->GetAbilitySystemComponent()))
			{
				ItemManager->AddItem(Item.ItemID, Item.Quantity);
			}
		}
	}
}

TArray<FItemPair> UEmberInteractableItemDropComponent::GetDropItemName(const FName& MonsterID,
                                                                       const UAbilitySystemComponent* AbilitySystemComponent)
{
	TArray<FItemPair> DropItems;

	if (TObjectPtr<UItemSubsystem> ItemSubsystem = UItemSystemLibrary::GetItemSubsystem())
	{
		DropItems = ItemSubsystem->GetDroppedItem(MonsterID, AbilitySystemComponent);
	}
    
	return DropItems;
}
