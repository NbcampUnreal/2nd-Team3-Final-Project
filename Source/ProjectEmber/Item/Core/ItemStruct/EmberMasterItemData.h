// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberItemEntry.h"
#include "EmberMasterItemData.generated.h"


struct FInstancedStructTypeKeyFuncs 
{
	using ElementType    = FInstancedStruct;
	using ElementInitType = FInstancedStruct;
	using KeyType        = const UScriptStruct*;
	using KeyInitType    = const UScriptStruct*;
	
	static const bool bAllowDuplicateKeys = false;

	static FORCEINLINE KeyType GetSetKey(const ElementType& Element)
	{
		return Element.GetScriptStruct();
	}

	static FORCEINLINE bool Matches(KeyType A, KeyType B)
	{
		return A == B;
	}

	static FORCEINLINE uint32 GetKeyHash(KeyType Key)
	{
		return ::GetTypeHash(Key);
	}
};


USTRUCT(BlueprintType)
struct FEmberMasterItemData : public FEmberItemEntry
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDisplayName = FText();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDescription = FText();

	TSet<FInstancedStruct, FInstancedStructTypeKeyFuncs> DataInstancedStructs = TSet<FInstancedStruct, FInstancedStructTypeKeyFuncs>();
    
	FEmberMasterItemData() = default;
    
	FEmberMasterItemData(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>());
	FEmberMasterItemData(const FEmberItemEntry& EmberItemEntry);
	
	FEmberMasterItemData(const FInstancedStruct& InInstancedStruct);

	void CashedMasterItemData();
	
	virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
	{
		OutInstancedStruct.InitializeAs<FEmberMasterItemData>(*this);
	}
	template<typename T>
	typename TEnableIf<TIsDerivedFrom<T, FTableRowBase>::Value, const T*>::Type FindData() const
	{

		if (const UScriptStruct* SlotScriptStruct = T::StaticStruct())
		{
			if (const FInstancedStruct* InstancedStruct = DataInstancedStructs.Find(SlotScriptStruct))
			{
				if (const T* DataStruct = InstancedStruct->GetPtr<T>())
				{
					return DataStruct;
				}
			}
		}
		return nullptr;

	}

	virtual void Clear() override
	{
		Super::Clear();
		ItemDisplayName = FText();
		ItemDescription = FText();
		DataInstancedStructs.Reset();
	}

};
