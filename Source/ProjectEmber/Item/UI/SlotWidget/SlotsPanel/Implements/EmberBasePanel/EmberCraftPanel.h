// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/EmberCraftStruct.h"
#include "Item/UI/SlotWidget/SlotsPanel/EmberBasePanel.h"
#include "EmberCraftPanel.generated.h"

class UEmberCraftComponent;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberCraftPanel : public UEmberBasePanel
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Craft")
	void InitData(UEmberCraftComponent* InCraftComponent);

	UFUNCTION(BlueprintCallable, Category = "Craft")
	void StartCraftingItem(int32 CraftingIndex);
	virtual void BindToManagerDelegates_Implementation() override;
	virtual void UnBindToManagerDelegates_Implementation() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	TArray<FCraftSelectInfo> CraftSelectInfos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	TObjectPtr<UEmberCraftComponent> CraftComponent;
};
