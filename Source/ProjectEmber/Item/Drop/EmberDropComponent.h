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
	void SetRandomItems(const UAbilitySystemComponent* AbilitySystemComponent = nullptr);

	// -- IEmberResourceProvider --
	virtual TMap<FName, int32> GetAllItemInfos_Implementation() override;
	
	virtual void TryConsumeResource_Implementation(const TArray<FItemPair>& InRequireItems) override;

	virtual TArray<FItemPair> RemoveResourceUntilAble_Implementation(const TArray<FItemPair>& InRequireItems) override;

	virtual bool bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drop")
	TObjectPtr<UEmberDropItemManager> DropManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	FName DropID;
};
