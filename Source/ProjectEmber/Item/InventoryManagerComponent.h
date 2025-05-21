// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EmberItemStruct.h"
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
	
	virtual int32 AddItemAndHandleOverflow_Implementation(FName ItemIDToAdd, int32 QuantityToAdd, FVector DropLocation, FRotator DropRotation);

    virtual int32 RemoveItemFromSlot_Implementation(int32 SlotIndex, int32 QuantityToRemove = 0);

    virtual void UseItemInSlot_Implementation(int32 SlotIndex);

	virtual int32 GetSlotMaxRow_Implementation() const;

	virtual void MovedInItemByAnotherProvider(int32 IndexTo, TScriptInterface<UEmberSlotDataProviderInterface> AnotherProvider, int32 IndexFrom, int32 Quantity);

	virtual FName GetSlotItemID_Implementation(int32 InIndex) const;
	
	virtual void MoveItemBySlot_Implementation(const FGameplayTag& SlotTag, int32 IndexTo, const TScriptInterface<UEmberSlotDataProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity);

    // --- 인벤토리 정보 조회 함수들 ---

    UFUNCTION(BlueprintPure, Category = "Inventory")
    const TArray<FInventorySlotData>& GetInventorySlots() const { return InventorySlots; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    FInventorySlotData GetSlotDataByIndex(int32 SlotIndex) const;

	virtual int32 GetSlotCount_Implementation() const;

	virtual FGameplayTag GetSlotType_Implementation() const;

	// --- 아이템 정리 관련 함수들 ---
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity);

	/*
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SortItem();*/
	
protected:
   /**
    * 
    * @param ItemIDToAdd 넣을 아이템ID 
    * @param QuantityToAdd 넣을 수량 
    * @param InSlotIndex -1인경우 있는곳에 넣고 못채운 나머지는 빈공간에 할당, 0 이상인경우 그 공간에 시도한다
    * @return 
    */
   int32 TryAddItemsToSlots(FName ItemIDToAdd, int32 QuantityToAdd, int32 InSlotIndex = -1);
	
    void InitializeInventorySlots();

	int32 FindEmptySlot() const;

    bool IsSlotFull(int32 SlotIndex) const;

    void HandleItemConsumption(const FConsumableInfoRow* ConsumeData);

	void SpawnDroppedItem(FName ItemIDToDrop, int32 QuantityToDrop, FVector SpawnLocation, FRotator SpawnRotation);

	virtual void BeginPlay() override;

public:

   UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChangedDelegate OnInventoryChanged;

protected:
	UPROPERTY(Transient)
	TObjectPtr<UItemSubsystem> ItemSubsystem;

	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slot", SaveGame) // SaveGame 필요시 추가
	TArray<FInventorySlotData> InventorySlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slot", meta = (ClampMin = "1"))
	int32 InventoryCapacity = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slot", meta = (ClampMin = "1"))
	int32 SlotMaxRow = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slot", meta = (ClampMin = "1", AllowPrivateAccess = true))
	FGameplayTag SlotTag;
	/*/** 월드에 드랍될 아이템 액터의 블루프린트 클래스 (에디터에서 설정) #1#
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Drop Item")
	TSubclassOf<AMyDroppedItemActor> DroppedItemActorClass;*/
};
