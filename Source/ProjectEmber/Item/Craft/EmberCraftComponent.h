// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberItemCollectorBoxCollision.h"
#include "EmberResourceProvider.h"
#include "Components/ActorComponent.h"
#include "EmberCraftComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberCraftComponent : public UEmberItemCollectorBoxCollision
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberCraftComponent();

	UFUNCTION(BlueprintCallable, Category="Craft")
	FItemPair CraftItem(const FName& InItemID);
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft|DataTables")
	TObjectPtr<UDataTable> CraftDataTable = nullptr;
	
};
