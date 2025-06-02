// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmberDropComponent.generated.h"


class UEmberDropItemManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberDropComponent();

	UFUNCTION(BlueprintCallable)
	void SetRandomItems();
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drop")
	TObjectPtr<UEmberDropItemManager> DropManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	FName DropID;
};
