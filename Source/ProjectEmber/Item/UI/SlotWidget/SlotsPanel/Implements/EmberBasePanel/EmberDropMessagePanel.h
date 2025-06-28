// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/UI/SlotWidget/SlotsPanel/EmberBasePanel.h"
#include "EmberDropMessagePanel.generated.h"

class UVerticalBox;
class UVerticalBoxSlot;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberDropMessagePanel : public UEmberBasePanel
{
	GENERATED_BODY()
public:
	virtual void BP_SetProvider_Implementation() override;

};
