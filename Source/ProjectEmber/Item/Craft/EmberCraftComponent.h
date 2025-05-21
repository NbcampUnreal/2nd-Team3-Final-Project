// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberResourceProvider.h"
#include "Components/ActorComponent.h"
#include "EmberCraftComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberCraftComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberCraftComponent();
	UFUNCTION(BlueprintCallable, Category="Craft")
	void InitResourceProvider(const TScriptInterface<IEmberResourceProvider> InResourceProvider);

	UFUNCTION(BlueprintCallable, Category="Craft")
	FItemPair CraftItem(const FName& InItemID);

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Craft|VisibleOnly")
	TScriptInterface<IEmberResourceProvider> ResourceProvider = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft|DataTables")
	TObjectPtr<UDataTable> CraftDataTable = nullptr;

};
