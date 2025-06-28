// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Item/Core/ItemStruct/EmberSlot.h"
#include "UObject/Object.h"
#include "EmberEquipmentSlot.generated.h"

USTRUCT(BlueprintType)
struct FEmberEquipmentSlot : public FEmberSlot
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FEquipmentInfoRow EquipmentInfo = FEquipmentInfoRow();

	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
	TArray<FItemEffectApplicationInfo> MainEffectInfos = TArray<FItemEffectApplicationInfo>();
	TArray<FActiveGameplayEffectHandle> MainEquipmentEffects = TArray<FActiveGameplayEffectHandle>();

	FEmberEquipmentSlot() = default;
	FEmberEquipmentSlot(const FName& InItemID, const int32 InQuantity = 0, const TArray<FItemEffectApplicationInfo>& InEnchantEffects = TArray<FItemEffectApplicationInfo>());
	FEmberEquipmentSlot(const FInstancedStruct& InInstancedStruct);

	void CashedEmberEquipmentData();

	virtual void Clear() override;
    
	virtual void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const override
	{
		OutInstancedStruct.InitializeAs<FEmberEquipmentSlot>(*this);
	}
};
