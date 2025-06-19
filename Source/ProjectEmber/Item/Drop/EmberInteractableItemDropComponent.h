// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Core/EmberItemStruct.h"
#include "EmberInteractableItemDropComponent.generated.h"


class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberInteractableItemDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberInteractableItemDropComponent();
	UFUNCTION(BlueprintCallable)
	virtual void DropItem(AActor* Actor);

protected:
	TArray<FItemPair> GetDropItemName(const FName& MonsterID, const UAbilitySystemComponent* AbilitySystemComponent = nullptr);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FName ItemName;
};
