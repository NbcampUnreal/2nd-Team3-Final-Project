// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseItemContainer.h"
#include "Item/Core/ItemStruct/EmberItemEntry.h"
#include "Item/UI/EmberSlotProviderInterface.h"
#include "EmberBaseSlotContainer.generated.h"

class UEmberSlotDataProviderInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemChangedDelegate, int32, SlotIndex, const FInstancedStruct&,
											 SlotData);
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberBaseSlotContainer : public UEmberBaseItemContainer, public IEmberSlotProviderInterface 
{
	GENERATED_BODY()
public:
	UEmberBaseSlotContainer();
	
	void InitSlotCount(int32 InSlotCount, int32 InSlotMaxRow);
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void AddSlotItem(FEmberItemEntry& InOutItemEntry, int32 InSlotIndex);
	virtual void AddSlotItem(const FName& InItemID, int32& InOutQuantity, int32 InSlotIndex, const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>());
	virtual void AddSlotItem(FInstancedStruct& InOutInstancedStruct, int32 InSlotIndex) ;

	/**
	 * 최대 수량을 넘어서 넣는경우
	 * return 값은 넣은 수량을 반환
	 * @return 넣은 수량
	 */
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual int32 AddSlotItemReturnApplied(const FEmberItemEntry& InItemEntry, int32 InSlotIndex) ;
	virtual int32 AddSlotItemReturnApplied(const FName& InItemID, const int32 InQuantity, int32 InSlotIndex, const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>()) ;
	virtual int32 AddSlotItemReturnApplied(const FInstancedStruct& InInstancedStruct, int32 InSlotIndex) ;

	/**
	* 최대 수량을 넘어서 넣는경우
	* InOutQuantity은 못넣은 수량으로 변화한다
	*/
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void RemoveSlotItem(int32& InOutQuantity, int32 InSlotIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual int32 RemoveSlotItemReturnApplied(const int32 InOutQuantity, int32 InSlotIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void SwapSlots(int32 SlotIndex1, int32 SlotIndex2);

	// --- 아이템 정리 관련 함수들 ---
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual int32 MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity);
	
	template <typename T>
	T* GetItemSlotInfo(const int32 ItemIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual FInstancedStruct GetInstancedItemSlotInfo(const int32 ItemIndex) const;

protected:

	virtual void CreateItemSlot(const FEmberItemEntry& InItemEntry, const int32 InItemIndex);
	virtual void DeleteItemSlot(const int32 InItemIndex);

	virtual bool bAbleAddItemSlot(const FInstancedStruct& InItemEntry, int32 InSlotIndex);


	// --- 아이템 정리 관련 헬퍼 ---
	/**
	* 
	* @return 슬롯에 채워진 양 , -1은 오류 혹은 다른 아이템
	*/
	int32 MergeSameItemSlot(int32 SlotIndexTo, int32 SlotIndexFromm, int32 MergeQuantity);
	
	// --- IEmberSlotProviderInterface ---
	virtual int32 GetSlotCount_Implementation() const override;
	virtual FInstancedStruct GetSlotItemInfo_Implementation(int32 InIndex) const override;
	virtual void AddItemByWidget_Implementation(FEmberItemEntry& InOutItem, int32 InSlotIndex = -1) override;
	virtual int32 RemoveItemFromSlot_Implementation(int32 InSlotIndex, int32 QuantityToRemove) override;
	virtual void MoveItemByWidget_Implementation(const FGameplayTag& SlotTag, int32 IndexTo, const TScriptInterface<UEmberSlotProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity = -1) override;
	virtual int32 GetSlotMaxRow_Implementation() const override;
	virtual void UseItemInSlot_Implementation(int32 SlotIndex) override;
	
public:
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemChangedDelegate OnItemChangedDelegate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", SaveGame) // SaveGame 필요시 추가
	TArray<FInstancedStruct> ItemSlots;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item") // SaveGame 필요시 추가
	int32 SlotCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item") // SaveGame 필요시 추가
	int32 SlotMaxRow = 0;
	
};

template <typename T>
T* UEmberBaseSlotContainer::GetItemSlotInfo(const int32 ItemIndex)
{
	FInstancedStruct InstancedStruct = GetInstancedItemSlotInfo(ItemIndex);
	return InstancedStruct.GetMutablePtr<T>();
}

