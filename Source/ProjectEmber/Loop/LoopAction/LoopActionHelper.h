// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LoopActionHelper.generated.h"

class AEmberMainHUD;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API ULoopActionHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	//// 사운드
	UFUNCTION(BlueprintCallable, Category = "Loop|Sound")
	static void SetSoundMixClassOverride(UObject* WorldContext, USoundMix* SoundMix, USoundClass* SoundClass, float Volume, float Pitch = 1.f, float FadeTime = 0.2f);

	UFUNCTION(BlueprintCallable, Category="Loop|Sound")
	static void PushSoundMix(UObject* WorldContextObject, USoundMix* SoundMix);

	UFUNCTION(BlueprintCallable, Category="Loop|Sound")
	static void PopSoundMix(UObject* WorldContextObject, USoundMix* SoundMix);

	UFUNCTION(BlueprintCallable, Category="Loop|Sound")
	static void ClearSoundMixModifiers(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="Loop|UI")
	static AEmberMainHUD* GetMainHUD(UObject* WorldContextObject);
	
};
