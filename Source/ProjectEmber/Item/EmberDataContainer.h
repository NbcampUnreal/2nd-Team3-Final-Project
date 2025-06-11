// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Craft/EmberResourceProvider.h"
#include "StructUtils/InstancedStruct.h"
#include "UI/SlotWidget/EmberSlotDataProviderInterface.h"
#include "Containers/Map.h"
#include "Containers/Queue.h"
#include "Item/Core/EmberCraftStruct.h"
#include "EmberDataContainer.generated.h"

struct FConsumableInfoRow;
struct FInventorySlotData;
class UAbilitySystemComponent;
class UItemSubsystem;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDataChangedDelegate, int32, SlotIndex, const FInstancedStruct&,
                                             SlotData);

UCLASS(ClassGroup=(Custom))
class PROJECTEMBER_API UEmberDataContainer : public UObject, public IEmberSlotDataProviderInterface, public IEmberResourceProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberDataContainer();

	void InitSlot(int32 SlotMax, int32 SlotMaxRow, TObjectPtr<AActor> InOwner);

	// --- 인벤토리 정보 조회 함수들 ---

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FInstancedStruct>& GetInventorySlots() const { return DataSlots; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FInstancedStruct GetSlotDataByIndex(int32 SlotIndex) const;

	// --- 크래프트 관련 함수 ---
	virtual TMap<FName, int32> GetAllItemInfos_Implementation() override;
	virtual bool bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems) override;

	void TryConsumeResource_Implementation(TArray<FItemPair>& InRequireItems);
	virtual TArray<FItemPair> RemoveResourceUntilAble_Implementation(const TArray<FItemPair>& InRequireItems) override;
	// --- 아이템 정리 관련 함수들 ---
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity);

	// --- 정보 전달체 관련 인터페이스 구현 ---
	virtual int32 AddItem_Implementation(const FItemPair& InItem, int32 InSlotIndex = -1) override;

	virtual TArray<FItemPair> AddItems_Implementation(const TArray<FItemPair>& Items) override;
	
	virtual int32 RemoveItemFromSlot_Implementation(int32 SlotIndex, int32 QuantityToRemove = 0) override;

	virtual void UseItemInSlot_Implementation(int32 SlotIndex) override;

	virtual int32 GetSlotMaxRow_Implementation() const override;
	
	virtual FName GetSlotItemID_Implementation(int32 InIndex) const override;
	
	virtual FInstancedStruct GetSlotItemInfo_Implementation(int32 InIndex) const override;
	
	virtual void MoveItemByWidget_Implementation(const FGameplayTag& SlotTag, int32 IndexTo, const TScriptInterface<UEmberSlotDataProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity) override;
	
	virtual int32 GetSlotCount_Implementation() const override;

	virtual FGameplayTag GetSlotType_Implementation() const override;
	
	int32 RemoveItemAutomatic(const FItemPair& InItem);

	/*
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SortItem();*/

	void InitOwner(TObjectPtr<AActor> InOwner);
protected:
	/**
	 * 
	 * @param FItemPair 넣을 아이템ID , 수량
	 * @param InSlotIndex -1인경우 있는곳에 넣고 못채운 나머지는 빈공간에 할당, 0 이상인경우 그 공간에 시도한다
	 * @return 
	 */
	virtual int32 TryAddItemsToSlots(const FItemPair& InItem, int32 InSlotIndex = -1);
	/**
	 * 
	 * @param FItemPair 넣을 아이템ID , 수량
	 * @param InSlotIndex -1인경우 있는곳에 넣고 못채운 나머지는 빈공간에 할당, 0 이상인경우 그 공간에 시도한다
	 * @return 
	 */
	virtual int32 TryAddItems(const FInstancedStruct& InItem, int32 InSlotIndex = -1);
	virtual int32 AddItemSlot(const FInstancedStruct& InItem, int32 InSlotIndex = -1);

	/**
	 * 보관하는 FStruct를 저장, 반드시 FEmberSlotData를 상속받은 FStruct로 저장할것
	 */
	virtual void InitializeInventorySlots();

	void HandleItemConsumption(const FConsumableInfoRow* ConsumeData);

	void SpawnDroppedItem(FName ItemIDToDrop, int32 QuantityToDrop, FVector SpawnLocation, FRotator SpawnRotation);

	virtual void MovedInItemByAnotherProvider(int32 IndexTo, TScriptInterface<UEmberSlotDataProviderInterface> AnotherProvider, int32 IndexFrom, int32 Quantity);

	/**
	 * 
	 * @param Slot 넣을 슬롯
	 * @param QuantityToAdd 넣을 양
	 * @return 슬롯에 채워진 양
	 */
	int32 AddItemSlotIndex(FEmberSlotData& Slot, int32 QuantityToAdd) const;
	

	/**
	 * 
	 * @return 슬롯에 채워진 양 , -1은 오류 혹은 다른 아이템
	 */
	int32 MergeSameItemSlot(int32 SlotIndexTo, int32 SlotIndexForm);
	void SwapInventorySlots(int32 SlotIndex1, int32 SlotIndex2);

	/**
	 * Index를 지정해서 최대 스택만큼만 저장
	 * @return 저장한 수량
	 */
	virtual int32 AddDataInIndex(const FItemPair& InItem, int32 InSlotIndex);
	virtual int32 AddDataInIndex(const FInstancedStruct& InItem, int32 InSlotIndex);
	void FindSlotIndexesByItemID(const FName& ItemID,  TQueue<int32>& OutSlotIndexes, int32 InBeginIndex = 0);
	void FindEmptySlotIndexes(TQueue<int32>& OutSlotIndexes) const;
	int32 FindEmptyFirstSlot() const;

public:

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnDataChangedDelegate OnDataChangedDelegate;

protected:
	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slot", SaveGame) // SaveGame 필요시 추가
	TArray<FInstancedStruct> DataSlots;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slot", SaveGame) // SaveGame 필요시 추가
	TMap<FName, FTotalItemInfo> TotalData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slot", meta = (ClampMin = "1"))
	int32 SlotCapacity = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slot", meta = (ClampMin = "1"))
	int32 SlotMaxRow = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slot", meta = (ClampMin = "1"))
	int32 ProviderOrder = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slot")
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> Owner = nullptr;
};
