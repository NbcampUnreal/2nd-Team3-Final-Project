// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/ItemTypes.h"
#include "UI/SlotWidget/EmberSlotDataProviderInterface.h"
#include "InventoryManagerComponent.generated.h"

struct FConsumableInfoRow;
class UItemSubsystem;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChangedDelegate, int32, SlotIndex, const FInventorySlotData&, SlotData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInventoryManagerComponent : public UActorComponent, public IEmberSlotDataProviderInterface
{
   GENERATED_BODY()

public:	
    UInventoryManagerComponent();
	
    UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual int32 AddItemAndHandleOverflow(FName ItemIDToAdd, int32 QuantityToAdd, FVector DropLocation, FRotator DropRotation) override;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual int32 RemoveItemFromSlot(int32 SlotIndex, int32 QuantityToRemove = 0) override;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void UseItemInSlot(int32 SlotIndex) override;

    // --- 인벤토리 정보 조회 함수들 ---

    UFUNCTION(BlueprintPure, Category = "Inventory")
    const TArray<FInventorySlotData>& GetInventorySlots() const { return InventorySlots; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool GetSlotDataByIndex(int32 SlotIndex, FInventorySlotData& OutSlotData) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual int32 GetSlotCount() const override;

	// --- 아이템 정리 관련 함수들 ---
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity) override;

	/*
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SortItem();*/
	
protected:
	int32 TryAddItemsToSlots(FName ItemIDToAdd, int32 QuantityToAdd);
	
    void InitializeInventorySlots();

	int32 FindEmptySlot() const;

    bool IsSlotFull(int32 SlotIndex) const;

    void HandleItemConsumption(const FConsumableInfoRow* ConsumeData);

	void SpawnDroppedItem(FName ItemIDToDrop, int32 QuantityToDrop, FVector SpawnLocation, FRotator SpawnRotation);

	virtual void BeginPlay() override;

protected:
	UPROPERTY(Transient)
	TObjectPtr<UItemSubsystem> ItemSubsystem;

	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "Inventory", SaveGame) // SaveGame 필요시 추가
	TArray<FInventorySlotData> InventorySlots;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 InventoryCapacity = 30;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChangedDelegate OnInventoryChanged;

	/*/** 월드에 드랍될 아이템 액터의 블루프린트 클래스 (에디터에서 설정) #1#
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Drop Item")
	TSubclassOf<AMyDroppedItemActor> DroppedItemActorClass;*/
};
