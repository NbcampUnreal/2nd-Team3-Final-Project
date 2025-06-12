// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AnimalSpeed.generated.h"

/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API UGA_AnimalSpeed : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_AnimalSpeed();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	void DownSpeedTime();

	UFUNCTION(BlueprintCallable)
	void EndSpeedChangTime();
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> StartEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> EndEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	float ApplyTime = 1.0;
	
private:
	FTimerHandle SpeedTimerHandle;
	float OriginSpeed;
};
