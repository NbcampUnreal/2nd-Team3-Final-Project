// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Drop/EmberInteractableItemDropComponent.h"
#include "EmberOreItemComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberOreItemComponent : public UEmberInteractableItemDropComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	bool bAbleDrop(UAbilitySystemComponent* AbilitySystemComponent);

	virtual void DropItem(AActor* Actor) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	int32 MiningLevel = 0;
};
