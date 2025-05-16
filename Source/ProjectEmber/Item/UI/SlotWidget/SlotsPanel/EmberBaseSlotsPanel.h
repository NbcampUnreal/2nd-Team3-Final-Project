// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/UI/SlotWidget/EmberSlotDataProviderInterface.h"
#include "EmberBaseSlotsPanel.generated.h"

class UGridPanel;
class UEmberBaseSlotWidget;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberBaseSlotsPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	UFUNCTION(BlueprintCallable)
	virtual void InitializePanel();

	UFUNCTION(BlueprintNativeEvent, Category="Slots Panel", meta=(DisplayName="Setup Delegate Bindings"))
	void BindToManagerDelegates();
	
	UFUNCTION(BlueprintNativeEvent, Category="Slots Panel", meta=(DisplayName="CreateAndArrageSlots"))
	void BP_CreateSlots();
	
	UFUNCTION(BlueprintNativeEvent, Category="Slots Panel", meta=(DisplayName="CreateAndArrageSlots"))
	void BP_SetProvider();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slots Panel")
	TScriptInterface<IEmberSlotDataProviderInterface> DataProvider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slots Panel")
	TArray<TObjectPtr<UEmberBaseSlotWidget>> Slots;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slots Panel")
	TSubclassOf<UEmberBaseSlotWidget> SlotClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UGridPanel> SlotsPanel;
};
