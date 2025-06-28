// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Core/EmberCraftStruct.h"
#include "EmberCraftWidget.generated.h"

class UEmberCraftComponent;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberCraftWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Craft")
	void InitCraft(UEmberCraftComponent* InCraftComponent, const FCraftSelectInfo& InCraftInfo, int32 InIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Craft")
	void StartCraftingItem();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	FCraftSelectInfo CraftInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	int32 CraftIndex;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	TObjectPtr<UEmberCraftComponent> CraftComponent;
};
