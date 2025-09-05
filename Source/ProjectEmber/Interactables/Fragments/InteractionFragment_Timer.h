// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Fragments/InteractionFragment.h"
#include "InteractionFragment_Timer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimerExpiredDelegate, AActor*, Interactor);

UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInteractionFragment_Timer : public UInteractionFragment
{
	GENERATED_BODY()
public:
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;

	virtual void EndInteraction_Implementation() override;

	void StopTimer();
	
	UPROPERTY(BlueprintAssignable, Category="Interaction|Timer")
	FTimerExpiredDelegate OnTimerExpiredEvent;
protected:
	UPROPERTY(EditAnywhere, Category = "Timer")
	float Duration = 1.0f;

	FTimerHandle TimerHandle;
	
	UFUNCTION()
	void OnTimerExpired(AActor* Interactor);
};
