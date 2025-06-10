// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberResourceProvider.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "EmberItemCollectorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberItemCollectorComponent : public UActorComponent,public IEmberResourceProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberItemCollectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void FindOverlappingResourceComponent();

	UFUNCTION()
	TArray<TWeakObjectPtr<UObject>> GetResourceProvider();
	
	UFUNCTION(BlueprintCallable)
	void SetResourceProvider(TScriptInterface<UEmberResourceProvider> Provider);

	UFUNCTION(BlueprintCallable)
	int32 DEBUG_GetResourceProviderNum();

	// --- IEmberResourceProvider ---
	virtual TMap<FName, int32> GetAllItemInfos_Implementation() override;
	
	virtual void TryConsumeResource_Implementation(const TArray<FItemPair>& InRequireItems) override;

	virtual bool bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems) override;

	virtual TArray<FItemPair> RemoveResourceUntilAble_Implementation(const TArray<FItemPair>& InRequireItems) override;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collectors|Collision")
	TObjectPtr<UBoxComponent> CollectRangeBox;
	
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UObject>> ResourceProviders;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collectors|Collision")
	float Radius;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collectors|Collision")
	FVector RelativeOffset;
	
};
