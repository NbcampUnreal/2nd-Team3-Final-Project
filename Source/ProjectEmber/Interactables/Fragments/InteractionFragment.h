// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionFragment.generated.h"

UCLASS(Abstract, Blueprintable, ClassGroup=(Interactor), meta=(BlueprintSpawnableComponent))
class UInteractionFragment : public UActorComponent
{
	GENERATED_BODY()

public:
	/** 실행 시도 → 성공 시 실행 */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void TryExecuteInteraction(AActor* Interactor);

	/** 실행 조건 검사 (추상적 조건 포함 가능) */
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	bool CanExecuteInteraction(AActor* Interactor) const;
	
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void ExecuteInteraction(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void EndInteraction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	bool bInteractionEnabled = true;

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void DisableInteraction() { bInteractionEnabled = false; }

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void EnableInteraction() { bInteractionEnabled = true; }

	UFUNCTION(BlueprintPure, Category="Interaction")
	bool IsInteractionEnabled() const { return bInteractionEnabled; }
};
