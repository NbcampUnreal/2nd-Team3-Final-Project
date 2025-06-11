// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberResourceProvider.h"
#include "Components/BoxComponent.h"
#include "EmberItemCollectorBoxCollision.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberItemCollectorBoxCollision : public UBoxComponent, public IEmberResourceProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberItemCollectorBoxCollision();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;
	
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
	
	virtual void GetItemInfo_Implementation(FEmberItemEntry& InItemEntry, FInstancedStruct& OutItemInfo) override;
	
	virtual void GetItemInfos_Implementation(TArray<FEmberItemEntry>& InItemEntries, TMap<FEmberItemKey, FInstancedStruct>& OutItemInfos) override;


	virtual void RemoveResourceUntilAble_Implementation(TArray<FItemPair>& InRequireItems) override;

public:
	
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UObject>> ResourceProviders;
	
};
