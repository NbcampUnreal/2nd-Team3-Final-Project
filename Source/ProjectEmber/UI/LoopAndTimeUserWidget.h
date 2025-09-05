// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/EmberUserWidget.h"
#include "LoopAndTimeUserWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API ULoopAndTimeUserWidget : public UEmberUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void Update();
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentLoop_Text;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Hour_Text;
};
