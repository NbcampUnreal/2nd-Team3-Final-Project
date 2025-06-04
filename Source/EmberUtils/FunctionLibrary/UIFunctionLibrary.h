// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIFunctionLibrary.generated.h"

struct FGameplayTag;
class UEmberLayerBase;
/**
 * 
 */
UCLASS()
class EMBERUTILS_API UUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static bool RegisterLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag, UUserWidget* Layer);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static UUserWidget* PushContentToLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag, const TSubclassOf<UUserWidget>& WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static void PopContentToLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static void ClearToLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static void FocusGame(APlayerController* OwningPlayer);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static void FocusUI(APlayerController* OwningPlayer, UUserWidget* WidgetToFocus,bool ShowMouseCursor, bool bIgnoreMovement = false, bool bGameLeftMouseInputLock = false);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static bool GetIsAbilityInputLock(APlayerController* OwningPlayer);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static bool GetIsGameMovementInputLock(APlayerController* OwningPlayer);

private:
	static FVector2D MousePos;
};


