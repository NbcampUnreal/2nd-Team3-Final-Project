// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Utility/AlsGameplayTags.h"
#include "MessageBus/MessageBus.h"
#include "LootActorBase.generated.h"

class UEmberInteractableItemDropComponent;

UENUM(BlueprintType)
enum class ELootAbilityType : uint8
{
	Harvest,
	Mineral,
	Tree
};

class AEmberCharacter;
class UAbilitySystemComponent;
class UGameplayAbility;
class UStaticMeshComponent;

UCLASS()
class PROJECTEMBER_API ALootActorBase : public AActor
{
	GENERATED_BODY()

public:
	ALootActorBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
	UFUNCTION(BlueprintCallable)
	void StartInteractAbility(APawn* InstigatorPawn);

	UFUNCTION(BlueprintCallable)
	void UpdateInteractAbility();

	UFUNCTION(BlueprintCallable)
	void CancelInteractAbility();

	UFUNCTION(BlueprintCallable)
	void CompleteInteractAbility();
	
	UFUNCTION(BlueprintCallable)
	void RefreshOverlayMode(APawn* InstigatorPawn);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Ability")
	void SetHoldTime(float HoldTime);

	void SetLootTypeToHoldTime(UGameplayAbility* GameplayAbility);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UGameplayAbility> InteractAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FGameplayTag InteractOverlayTag{AlsOverlayModeTags::Default};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing")
	int32 SwingCount{1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LootType")
	ELootAbilityType LootAbilityType{ELootAbilityType::Tree};
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	FGameplayTagContainer GameplayTagContainer;
	
private:
	void SetCharacterRotation(AEmberCharacter* EmberCharacter, const float YawAngle);
	
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;
	FGameplayTag PreOverlayTag;
	bool bIsAbilityEnded{false};

private: /* MessageBus */
	void MessageBusSubscribe();
	void MessageBusUnsubscribe();
	void ReceiveMessage(const FName MessageType, UObject* Payload);
	
	FMessageDelegate MessageDelegateHandle;

protected:
	UPROPERTY(EditAnywhere, Blueprintable, Category = "Item")
	TObjectPtr<UEmberInteractableItemDropComponent> ItemDropComponent;
};
