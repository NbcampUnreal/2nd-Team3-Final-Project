// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Craft/EmberResourceProvider.h"
#include "EmberDropComponent.generated.h"


class UEmberDropItemManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberDropComponent : public UActorComponent, public IEmberResourceProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberDropComponent();

	UFUNCTION(BlueprintCallable)
	void AddRandomItemToPlayer();
	
	UFUNCTION(BlueprintCallable)
	void SetRandomItems(const UAbilitySystemComponent* AbilitySystemComponent = nullptr);

	UFUNCTION(BlueprintCallable)
	void ResetItems();

	// -- IEmberResourceProvider --
	virtual TMap<FName, int32> GetAllItemInfos_Implementation() override;
	
	virtual void TryConsumeResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems) override;

	virtual void RemoveResourceUntilAble_Implementation(TArray<FEmberItemEntry>& InRequireItems) override;
	virtual bool bConsumeAbleResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drop")
	TObjectPtr<UEmberDropItemManager> DropManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	FName DropID;
};
