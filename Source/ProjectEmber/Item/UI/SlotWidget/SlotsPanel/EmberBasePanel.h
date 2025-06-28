// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/PanelWidget.h"
#include "Item/UI/EmberSlotProviderInterface.h"
#include "EmberBasePanel.generated.h"

class UItemDetailWidget;

class UEmberBaseSlotWidget;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberBasePanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION(BlueprintCallable)
	virtual void InitializePanel();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Slots Panel", meta=(DisplayName="Setup Delegate Bindings"))
	void BindToManagerDelegates();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Slots Panel", meta=(DisplayName="Setup Delegate Bindings"))
	void UnBindToManagerDelegates();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Slots Panel", meta=(DisplayName="CreateSlots"))
	void BP_InitSlots();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Slots Panel", meta=(DisplayName="SetProvider"))
	void BP_SetProvider();

	UFUNCTION(BlueprintNativeEvent,  BlueprintCallable)
	void SlotChanged(int32 InIndex, const FInstancedStruct& InSlotData);
	UFUNCTION(BlueprintCallable)
	virtual void UpdateAllSlots();
	UFUNCTION(BlueprintCallable)
	virtual void UpdateSlotByIndex(int32 SlotIndex);
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Slots Panel")
	TScriptInterface<IEmberSlotProviderInterface> DataProvider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slots Panel")
	TArray<TObjectPtr<UEmberBaseSlotWidget>> Slots;
	
};
