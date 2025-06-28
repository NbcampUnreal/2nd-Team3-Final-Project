// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Core/ItemCraftType.h"
#include "Item/UI/SlotWidget/Slot/EmberBaseSlotWidget.h"
#include "EmberResourceSlot.generated.h"

struct FEmberMasterItemData;
class UEmberCraftComponent;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberResourceSlot : public UEmberBaseSlotWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	float GetPercentage();

	virtual void UpdateSlot() override;
	UFUNCTION(BlueprintCallable)
	void InitCraftComponent(TScriptInterface<IEmberSlotProviderInterface> InDataProvider, const int32 InSlotIndex, const int32 InRequireIndex);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentQuantity = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequireIndex = 0;
};
