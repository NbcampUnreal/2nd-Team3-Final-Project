// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StreamingSourceProvider.generated.h"


/**
 * 
 */
UCLASS(BlueprintType)
class PROJECTEMBER_API UStreamingSourceProvider : public UObject, public IWorldPartitionStreamingSourceProvider
{
	GENERATED_BODY()

public:
	virtual bool GetStreamingSources(TArray<FWorldPartitionStreamingSource>& OutSources) const override;
	
	UFUNCTION(BlueprintCallable, Category=Streaming)
	void SetLocation(const FVector& Location);
	
	UFUNCTION(BlueprintCallable, Category=Streaming)
	void SetLocations(const TArray<FVector>& Locations);

	UFUNCTION(BlueprintCallable, Category=Streaming)
	void AddLocation(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category=Streaming)
	void ClearLocations();
	
private:
	UPROPERTY()
	TArray<FVector> Locations;
};
