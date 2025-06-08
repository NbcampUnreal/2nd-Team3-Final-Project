// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseSlotsPanel.h"
#include "EmberDropItemSlotsPanel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberDropItemSlotsPanel : public UEmberBaseSlotsPanel
{
	GENERATED_BODY()

public:
	virtual void BP_SetProvider_Implementation() override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
};
