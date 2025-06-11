// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Core/EmberWidgetSlotData.h"
#include "ItemDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UItemDetailWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	FString GetEffectsName() const;
private:
	UFUNCTION(BlueprintCallable)
	void SetWidgetMousePosition();
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	FEmberWidgetSlotData EmberWidgetSlotData = FEmberWidgetSlotData();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slot")
	FTimerHandle MouseTimerHandle;
};
