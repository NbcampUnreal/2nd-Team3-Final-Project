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
	static bool RegisterLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag, UEmberLayerBase* Layer);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static UUserWidget* PushContentToLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag, const TSubclassOf<UUserWidget>& WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static void PopContentToLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static void ClearToLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag);

	UFUNCTION(BlueprintCallable, Category = "UIFunctionLibrary")
	static void FocusGame(APlayerController* OwningPlayer);
};
