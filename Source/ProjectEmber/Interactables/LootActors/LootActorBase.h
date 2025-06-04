// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Utility/AlsGameplayTags.h"
#include "LootActorBase.generated.h"

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|StateMapping")
	TSubclassOf<UGameplayAbility> InteractAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|StateMapping")
	FGameplayTag InteractOverlayTag{AlsOverlayModeTags::Default};

private:
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;
};
