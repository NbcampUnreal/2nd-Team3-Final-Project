// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMBERUTILS_API UCombatFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Applies a global time dilation for the specified duration, then resets to normal time.
	 * @param World - The world context.
	 * @param TimeDilation - New global time dilation (e.g., 0.5 for half speed).
	 * @param Duration - How long the dilation lasts before resetting.
	 */
	UFUNCTION(BlueprintCallable, Category="TimeDilation", meta=(WorldContext="World"))
	static void ApplyGlobalTimeDilation(UWorld* World, float TimeDilation, float Duration);

	/**
	 * Applies a custom time dilation to a specific actor for the specified duration, then resets it.
	 * @param TargetActor - The actor to slow down.
	 * @param TimeDilation - New actor time dilation (e.g., 0.5 for half speed).
	 * @param Duration - How long the dilation lasts before resetting.
	 */
	UFUNCTION(BlueprintCallable, Category="TimeDilation")
	static void ApplyActorTimeDilation(AActor* TargetActor, float TimeDilation, float Duration);

private:
	// Helpers to reset to normal speed
	static void ResetGlobalTimeDilation(UWorld* World);
	static void ResetActorTimeDilation(AActor* TargetActor);
};
