// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/EmberTmpStruct.h"
#include "Item/Core/ItemStruct/EmberItemEntry.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "EmberBaseItemContainer.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTEMBER_API UEmberBaseItemContainer : public UObject
{
	GENERATED_BODY()
	
public:

	/**
	* 최대 수량을 넘어서 넣는경우
	* InOutQuantity은 못넣은 수량으로 변화한다
	* 	* 이 함수는 전달체로서 로직은 ReturnApplied에서 실행
	*/
	virtual void AddItem(FInstancedStruct& InOutInstancedStruct);

	/**
	 * 최대 수량을 넘어서 넣는경우
	 * return 값은 넣은 수량을 반환
	 * @return 넣은 수량
	 */
	virtual int32 AddItemReturnApplied(const FInstancedStruct& InInstancedStruct);
	
	/**
	* 최대 수량을 넘어서 빼는경우
	* InOutQuantity은 못넣은 수량으로 변화한다
	* 이 함수는 전달체로서 로직은 ReturnApplied에서 실행
	*/
	virtual void RemoveItem(FInstancedStruct& InOutInstancedStruct);
	/**
	* 최대 수량을 넘어서 빼는경우
	 * return 값은 못넣은 수량으로 반환
	 * @return 못넣은 수량
	 */
	virtual int32 RemoveItemReturnApplied(const FInstancedStruct& InBaseItem);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitOwner(AActor* InOwner);
protected:
	
	virtual void CreateItem(const FEmberItemEntry& InItemEntry);
	virtual void DeleteItem(const FEmberItemEntry& InItemEntry);


	// --- 헬퍼 함수 ---
	virtual FInstancedStruct* GetInstancedItemInfo(const FName& InItemID,  const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>());
	virtual FInstancedStruct* GetInstancedItemInfo(const FEmberItemKey& InBaseItem);

	template <typename T>
	T* GetItemInfo(const FName& InItemID,  const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>());
	template <typename T>
	T* GetItemInfo(const FEmberItemEntry& InItemEntry);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", SaveGame) // SaveGame 필요시 추가
	TMap<FEmberItemKey, FInstancedStruct> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item") // SaveGame 필요시 추가
	TObjectPtr<AActor> Owner;

	// --- 헬퍼 함수 ---
	
public:
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void AddItem(FEmberItemEntry& InOutItemEntry);
	virtual void AddItem(const FName& InItemID, int32& InOutQuantity, const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>());

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual int32 AddItemReturnApplied(const FEmberItemEntry& InItemEntry);
	virtual int32 AddItemReturnApplied(const FName& InItemID, const int32 InQuantity, const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>());

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void RemoveItem(FEmberItemEntry& InOutItemEntry);
	virtual void RemoveItem(const FName& InItemID, int32& InOutQuantity, const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>());


	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual int32 RemoveItemReturnApplied(const FEmberItemEntry& InItemEntry);
	virtual int32 RemoveItemReturnApplied(const FName& InItemID, const int32 InQuantity, const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>());


};

template <typename T>
T* UEmberBaseItemContainer::GetItemInfo(const FName& InItemID,  const TArray<FItemEffectApplicationInfo>& InEnchants)
{
	if (FInstancedStruct* InstancedStruct = GetInstancedItemInfo(InItemID, InEnchants))
	{
		return InstancedStruct->GetMutablePtr<T>();
	}
	return nullptr;
}

template <typename T>
T* UEmberBaseItemContainer::GetItemInfo(const FEmberItemEntry& InItemEntry)
{
	if (FInstancedStruct* InstancedStruct = GetInstancedItemInfo(InItemEntry.CreateItemKey()))
	{
		return InstancedStruct->GetMutablePtr<T>();
	}
	return nullptr;
}
