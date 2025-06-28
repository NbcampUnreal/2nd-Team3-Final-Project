// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/ItemStruct/EmberMasterItemData.h"
#include "UObject/Object.h"
#include "EmberDropMessageSlot.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FEmberDropMessageSlot : public FEmberMasterItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FSlotInfoRow SlotInfo = FSlotInfoRow();

	float CurrentTime = 0.f;
	
	FEmberDropMessageSlot() = default;
	FEmberDropMessageSlot(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>());
	FEmberDropMessageSlot(const FEmberItemEntry& EmberItemEntry);
	FEmberDropMessageSlot(const FInstancedStruct& InInstancedStruct);
	
	void CashedDropMessageSlotData();

	virtual void Clear() override
	{
		Super::Clear();
		SlotInfo = FSlotInfoRow();
	}
    
	virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
	{
		OutInstancedStruct.InitializeAs<FEmberDropMessageSlot>(*this);
	}
};
