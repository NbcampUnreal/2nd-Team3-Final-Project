// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/EmberItemStruct.h"
#include "Item/Core/ItemTypes.h"
#include "EmberItemEntry.generated.h"

/**
 * 
 */

// 아이템 정보 전달체
USTRUCT(BlueprintType)
struct FEmberItemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", SaveGame)
	FName ItemID = NAME_None;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", SaveGame)
	int32 Quantity = 0;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", SaveGame)
	int32 MaxQuantity = ITEM_SYSTEM_MAX_STACK;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", SaveGame)
	TArray<FItemEffectApplicationInfo> Enchants = TArray<FItemEffectApplicationInfo>();

	FEmberItemEntry() = default;
	FEmberItemEntry(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchants = TArray<FItemEffectApplicationInfo>()) : ItemID(InItemID), Quantity(InQuantity), Enchants(InEnchants) {} ;
	FEmberItemEntry(const FInstancedStruct& InInstancedStruct);

	virtual ~FEmberItemEntry() = default;

	/**
	 * 최대 수량을 넘어서 넣는경우
	 * InOutQuantity은 못넣은 수량으로 변화한다
	 */
	virtual void AddQuantity(int32& InOutQuantity);
	/**
	 * 현재 가진양보다 많이 없앨경우
	 * InOutQuantity은 제거하지 못한 수량으로 변화한다
	 */
	virtual void RemoveQuantity(int32& InOutQuantity);
	/**
	 * 최대 수량을 넘어서 넣는경우
	 * return 값은 넣은 수량을 반환
	 * @return 넣은 수량
	 */
	virtual int32 AddQuantityReturnApplied(const int32 InQuantity);
	/**
	 * 현재 가진양보다 많이 없앨경우
	 * return 값은 제거한 수량을 반환
	 * @return 제가한 수량
	 */
	virtual int32 RemoveQuantityReturnApplied(const int32 InQuantity);
    
	virtual FEmberItemKey CreateItemKey() const;
    
	virtual FInstancedStruct CreateInstancedStruct() const;
    
	virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const
	{
		OutInstancedStruct.InitializeAs<FEmberItemEntry>(*this);
	}
    
	virtual void Clear()
	{
		ItemID = NAME_None;
		Quantity = 0;
		Enchants.Reset();
	}

	virtual bool bIsEmpty() const;
    
	virtual bool bIsFull() const;
    
	virtual bool bAbleAddQuantity(const int32 InQuantity) const;
    
	virtual bool bAbleRemoveQuantity(const int32 InQuantity) const;

	bool operator==(const FEmberItemEntry& InOther) const
	{
		return this->CreateItemKey() == InOther.CreateItemKey();
	};
	bool operator==(const FEmberItemKey& InOther) const
	{
		return this->CreateItemKey() == InOther;
	};
};
