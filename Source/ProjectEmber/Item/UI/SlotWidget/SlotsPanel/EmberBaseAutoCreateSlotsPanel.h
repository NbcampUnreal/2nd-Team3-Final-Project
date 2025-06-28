// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBasePanel.h"
#include "EmberBaseAutoCreateSlotsPanel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberBaseAutoCreateSlotsPanel : public UEmberBasePanel
{
	GENERATED_BODY()
public:
	virtual void BP_InitSlots_Implementation() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slots Panel")
	TSubclassOf<UEmberBaseSlotWidget> SlotClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slots Panel")
	FMargin SlotMargin = FMargin(5.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UPanelWidget> SlotsPanel;
};
