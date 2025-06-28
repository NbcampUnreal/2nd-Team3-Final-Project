#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "EMSActorSaveInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "Item/Core/ItemStruct/EmberItemEntry.h"
#include "EmberPlayerState.generated.h"

UCLASS()
class PROJECTEMBER_API AEmberPlayerState : public APlayerState, public IAbilitySystemInterface, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	AEmberPlayerState();

	virtual void BeginPlay() override;
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	virtual void ActorPreSave_Implementation() override;
	UFUNCTION()
	virtual void ActorLoaded_Implementation() override;
	void HandleAbilityFailed(const UGameplayAbility* FailedAbility, const FGameplayTagContainer& FailureTags);
	
	void GameMenuWidgetLoaded();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<class UEmberPlayerAttributeSet> PlayerAttributeSet;
	UPROPERTY()
	TObjectPtr<class UEmberCharacterAttributeSet> AttributeSet;
	UPROPERTY()
	TObjectPtr<class UEmberItemAttributeSet> ItemAttributeSet;

	
	UPROPERTY(SaveGame)
	TMap<FName, int32> QuestProgress;
	
	UPROPERTY(SaveGame)
	TArray<FEmberItemEntry> InventoryProgress;
	
	UPROPERTY(SaveGame)
	TArray<FEmberItemEntry> EquipmentProgress;
	
	UPROPERTY(SaveGame)
	TArray<FEmberItemEntry> QuickSlotProgress;
};
