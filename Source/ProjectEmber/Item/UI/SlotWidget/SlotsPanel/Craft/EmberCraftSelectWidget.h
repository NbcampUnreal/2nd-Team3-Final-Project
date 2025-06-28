// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Core/EmberCraftStruct.h"
#include "EmberCraftSelectWidget.generated.h"

struct FEmberMasterItemData;
class UEmberCraftComponent;
/**
 * 
 */


UCLASS()
class PROJECTEMBER_API UEmberCraftSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Craft")
	void InitData(UEmberCraftComponent* InCraftComponent);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	TArray<FCraftSelectInfo> CraftSelectInfos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	TObjectPtr<UEmberCraftComponent> CraftComponent;
};

