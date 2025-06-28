// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Core/ItemCraftType.h"
#include "EmberResourceWidget.generated.h"

class UEmberCraftComponent;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberResourceWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Craft")
	void InitCraft(UEmberCraftComponent* InCraftComponent, const TArray<FCraftPair>& InRequestInfos);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	TArray<FCraftPair> RequestInfos;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	TObjectPtr<UEmberCraftComponent> CraftComponent;
};
