// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "EmberLayerBase.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJECTEMBER_API UEmberLayerBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	UUserWidget* PushWidget(const TSubclassOf<UUserWidget>& WidgetClass);

	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	void PopWidget();

	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	void ClearStack();

	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	UUserWidget* GetTopWidget();

	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	void CollapseTopWidget();

	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	void ShowTopWidget();

	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	FText GetStackListNames();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="UI|Layer")
	void Input_ToggleLayer(FName ChildName);

protected:
	UPROPERTY(BlueprintReadOnly, Category="UI|Layer")
	TArray<TObjectPtr<UUserWidget>> Stack;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UBorder* Border;
};
