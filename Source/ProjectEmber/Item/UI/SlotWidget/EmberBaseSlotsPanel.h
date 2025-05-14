// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberSlotDataProviderInterface.h"
#include "Blueprint/UserWidget.h"
#include "EmberBaseSlotsPanel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberBaseSlotsPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitializePanel(TScriptInterface<IEmberSlotDataProviderInterface> InDataProvider);

	UFUNCTION(BlueprintImplementableEvent, Category="Slots Panel", meta=(DisplayName="Setup Delegate Bindings"))
	void BP_BindToManagerDelegates();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Slots Panel", meta=(DisplayName="CreateAndArrageSlots"))
	void BP_CreateAndArrageSlots();
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TScriptInterface<IEmberSlotDataProviderInterface> DataProvider;

};
