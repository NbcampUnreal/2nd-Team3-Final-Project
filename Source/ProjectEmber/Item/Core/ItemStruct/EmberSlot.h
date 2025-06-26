// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberMasterItemData.h"
#include "EmberSlot.generated.h"

USTRUCT(BlueprintType)
struct FEmberSlot : public FEmberMasterItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FSlotInfoRow SlotInfo = FSlotInfoRow();

	FEmberSlot() = default;
	FEmberSlot(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>());
	FEmberSlot(const FEmberItemEntry& EmberItemEntry);
	FEmberSlot(const FInstancedStruct& InInstancedStruct);

	void CashedEmberSlotData();

	virtual void Clear() override
	{
		Super::Clear();
		SlotInfo = FSlotInfoRow();
	}
    
	virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
	{
		OutInstancedStruct.InitializeAs<FEmberSlot>(*this);
	}
};