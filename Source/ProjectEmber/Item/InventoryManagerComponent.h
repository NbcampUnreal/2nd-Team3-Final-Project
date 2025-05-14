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

protected:
    virtual void BeginPlay() override;

    UPROPERTY(Transient)
    TObjectPtr<UItemSubsystem> ItemSubsystem;

    UPROPERTY(Transient)
    TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, Category = "Inventory", SaveGame) // SaveGame 필요시 추가
    TArray<FInventorySlotData> InventorySlots;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (ClampMin = "1"))
    int32 InventoryCapacity = 30;

	/*/** 월드에 드랍될 아이템 액터의 블루프린트 클래스 (에디터에서 설정) #1#
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Drop Item")
	TSubclassOf<AMyDroppedItemActor> DroppedItemActorClass;*/
public:
	
    UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItemAndHandleOverflow(FName ItemIDToAdd, int32 QuantityToAdd, FVector DropLocation, FRotator DropRotation);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 RemoveItemFromSlot(int32 SlotIndex, int32 QuantityToRemove = 0);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UseItemInSlot(int32 SlotIndex);

    // --- 인벤토리 정보 조회 함수들 ---

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetInventoryCapacity() const { return InventoryCapacity; }
    
    UFUNCTION(BlueprintPure, Category = "Inventory")
    const TArray<FInventorySlotData>& GetInventorySlots() const { return InventorySlots; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool GetSlotDataByIndex(int32 SlotIndex, FInventorySlotData& OutSlotData) const;

protected:
	/**
 * 내부적으로 인벤토리 슬롯에 아이템을 추가하려고 시도하고, 실제로 추가된 수량을 반환합니다.
 * (기존 AddItem 함수의 로직이 여기로 이동)
 */
	int32 TryAddItemsToSlots(FName ItemIDToAdd, int32 QuantityToAdd);
    void InitializeInventorySlots();

	int32 FindEmptySlot() const;

    bool IsSlotFull(int32 SlotIndex) const;

    void HandleItemConsumption(const FConsumableInfoRow* ConsumeData);

	void SpawnDroppedItem(FName ItemIDToDrop, int32 QuantityToDrop, FVector SpawnLocation, FRotator SpawnRotation);

public:
   virtual int32 GetSlotCount() const override;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
   FOnInventoryChangedDelegate OnInventoryChanged;
};
