// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TestFood.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractiveObject : public UInterface
{
	GENERATED_BODY()
};

class IInteractiveObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent , Category = "InteractiveObject")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent , Category = "InteractiveObject")
	FGameplayTag GetTag();

	
};

UCLASS()
class PROJECTEMBER_API ATestFood : public AActor, public IInteractiveObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestFood();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FGameplayTag GetTag();
	virtual FGameplayTag GetTag_Implementation() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameplayTag")
	FGameplayTag GameplayTag;
};
