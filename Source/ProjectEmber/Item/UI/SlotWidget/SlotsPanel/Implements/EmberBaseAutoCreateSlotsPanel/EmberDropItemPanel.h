// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/UI/SlotWidget/SlotsPanel/EmberBaseAutoCreateSlotsPanel.h"
#include "EmberDropItemPanel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberDropItemPanel : public UEmberBaseAutoCreateSlotsPanel
{
	GENERATED_BODY()
	
public:
	virtual void BP_SetProvider_Implementation() override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

};
