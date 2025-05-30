// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AnimalDeath.generated.h"

/**
 * 
 */
UCLASS()
class EMBERABILITYSYSTEM_API UGA_AnimalDeath : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_AnimalDeath();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AnimalDeath")
	UAnimMontage* Montage;

protected:
	/**
	* ActivateAbility은 "Triggers"에 등록된 Gameplay Event Tag로만 호출
	* TriggerEventData->OptionalObject에 HarvestTarget을 받아와 몽타주를 재생
	* 완료시 Target 액터 삭제
	 */
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> EffectToApply;
	
private:
	UFUNCTION()
	void OnCompleteCallback();
	
	TWeakObjectPtr<const AActor> Instigator;
};
