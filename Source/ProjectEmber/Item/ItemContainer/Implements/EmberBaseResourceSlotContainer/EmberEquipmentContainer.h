// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Item/ItemContainer/EmberBaseResourceSlotContainer.h"
#include "Item/ItemContainer/EmberBaseSlotContainer.h"
#include "EmberEquipmentContainer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberEquipmentContainer : public UEmberBaseResourceSlotContainer
{
	GENERATED_BODY()
	
public:
	UEmberEquipmentContainer();
	
	UFUNCTION(BlueprintCallable)
	void SetOwnerAbilitySystemComponent(UAbilitySystemComponent* InOwnerAbilitySystemComponent);

	virtual bool bAbleAddItemSlot(const FInstancedStruct& InItemEntry, int32 InSlotIndex) override;

	bool bIsEquipmentTag(const FGameplayTag& InTag) const;
	virtual void CreateItemSlot(const FEmberItemEntry& InItemEntry, int32 InItemIndex) override;
	int32 GetSlotIndex(const FGameplayTag& InTag) const;

	virtual int32 RemoveSlotItemReturnApplied(const int32 InOutQuantity, int32 InSlotIndex = -1) override;

protected:
	void RemoveEffect(int32 InSlotIndex);
	void ActiveEffect(int32 InSlotIndex);

protected:
	FGameplayTagContainer SlotGameplayTags;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
	
	TArray<TArray<FActiveGameplayEffectHandle>> EquipmentEffects = TArray<TArray<FActiveGameplayEffectHandle>>();
};
