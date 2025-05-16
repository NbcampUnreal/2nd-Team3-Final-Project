// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EmberItemStruct.h"
#include "UserItemManger.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UUserItemManger : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUserItemManger();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	FEmberItemInfo GetQuickSlotInfo(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	void UseQuickSlotInfo(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	FEmberItemInfo GetInventorySlotInfo(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	void UseInventorySlotInfo(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	void AddItem(FName ItemID, int32 Quantity);
	
	const class UInventoryManagerComponent* GetInventoryManager() const;
	
	UInventoryManagerComponent* GetInventoryManager();
	
	const class UQuickSlotManagerComponent* GetQuickSlotManagerComponent() const;

	UQuickSlotManagerComponent* GetQuickSlotManagerComponent();
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager Component")
	TObjectPtr<UInventoryManagerComponent> InventoryManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager Component")
	TObjectPtr<UQuickSlotManagerComponent> QuickSlotManager;
	
};
