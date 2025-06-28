// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberEquipmentSlot.h"

#include "AbilitySystemComponent.h"

FEmberEquipmentSlot::FEmberEquipmentSlot(const FName& InItemID, const int32 InQuantity,
                                         const TArray<FItemEffectApplicationInfo>& InEnchantEffects) : FEmberSlot(InItemID, InQuantity, InEnchantEffects)
{
	CashedEmberEquipmentData();
}

FEmberEquipmentSlot::FEmberEquipmentSlot(const FInstancedStruct& InInstancedStruct) : FEmberSlot(InInstancedStruct)
{
	if (const FEmberEquipmentSlot* InDataInfoRow = InInstancedStruct.GetPtr<FEmberEquipmentSlot>())
	{
		EquipmentInfo = InDataInfoRow->EquipmentInfo;
	}
	else
	{
		CashedEmberEquipmentData();
	}
}

void FEmberEquipmentSlot::CashedEmberEquipmentData()
{
	if (const FEquipmentInfoRow* InEquipmentInfoRow = FindData<FEquipmentInfoRow>())
	{
		EquipmentInfo = *InEquipmentInfoRow;
		for (FDataTableRowHandle& RowHandle : EquipmentInfo.MainEffects)
		{
			if (const FItemEffectApplicationInfo* ItemEffectApplicationInfo = RowHandle.GetRow<FItemEffectApplicationInfo>(TEXT("Effect")))
			{
				MainEffectInfos.Add(*ItemEffectApplicationInfo);
			}
		}
	}
}

void FEmberEquipmentSlot::Clear()
{
	Super::Clear();
	for (FActiveGameplayEffectHandle& Effect : MainEquipmentEffects)
	{
		if (Effect.IsValid())
		{
			OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(Effect);
		}
	}
	MainEquipmentEffects.Reset();
	MainEffectInfos.Reset();
		
	EquipmentInfo = FEquipmentInfoRow();
}

