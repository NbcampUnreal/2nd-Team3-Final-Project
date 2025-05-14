#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_InteractHarvest.generated.h"

UCLASS()
class EMBERABILITYSYSTEM_API UGA_InteractHarvest : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_InteractHarvest();
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootHarvest")
	UAnimMontage* Montage;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
private:
	UFUNCTION()
	void OnCompleteCallback();
	UFUNCTION()
	void OnInterruptedCallback();
	
	TWeakObjectPtr<const AActor> HarvestTarget;
};
