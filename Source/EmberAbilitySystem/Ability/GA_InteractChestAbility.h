#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_InteractChestAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;

UENUM(BlueprintType)
enum class EChestState : uint8
{
	Opening,
	Opened,
	Picking,
	Closing
};

UCLASS()
class EMBERABILITYSYSTEM_API UGA_InteractChestAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_InteractChestAbility();
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chest")
	UAnimMontage* OpenMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chest")
	UAnimMontage* PickupMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chest")
	UAnimMontage* CloseMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chest")
	TSubclassOf<class UUserWidget> ChestWidgetClass;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	void PlayMontage(UAnimMontage* Montage, FName TaskName);
	
	UFUNCTION()
	void OnOpenFinished();
	UFUNCTION()
	void OnPickupEvent(FGameplayEventData Payload);
	UFUNCTION()
	void OnPickFinished();
	UFUNCTION()
	void OnCloseEvent(FGameplayEventData Payload);
	UFUNCTION()
	void OnCloseFinished();

	EChestState State;
	
	FGameplayTag PickupTag;
	FGameplayTag CloseTag;
	
	TObjectPtr<class UUserWidget> ChestWidget;
	TObjectPtr<UAbilityTask_PlayMontageAndWait> CurrentMontageTask;
};