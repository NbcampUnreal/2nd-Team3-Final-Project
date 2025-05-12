// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LayerDebugger.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API ULayerDebugger : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|HUD")
	void SetChangedLayer();
};
