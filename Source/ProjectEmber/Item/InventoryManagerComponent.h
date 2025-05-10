// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagerComponent.generated.h"

struct FConsumableComponentRow;
class UItemSubsystem;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FInventorySlotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot")
	FName ItemID = NAME_None;

	// 해당 슬롯에 있는 아이템의 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot", meta = (ClampMin = "0"))
	int32 Quantity = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	int32 MaxStackSize = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Slot")
	TOptional<FDataTableRowHandle> ConsumableComponentHandle;
	
	bool IsEmpty() const { return ItemID.IsNone() || Quantity <= 0; }
	void Clear()
	{
		ItemID = NAME_None;
		Quantity = 0;
		MaxStackSize = 1;
		ConsumableComponentHandle.Reset();
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInventoryManagerComponent : public UActorComponent
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

public:
	
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 AddItem(FName ItemIDToAdd, int32 QuantityToAdd);

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
    void InitializeInventorySlots();

	int32 FindEmptySlot() const;

    bool IsSlotFull(int32 SlotIndex) const;

    void HandleItemConsumption(const FConsumableComponentRow* ConsumeData);
		
};
