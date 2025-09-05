// Fill out your copyright notice in the Description page of Project Settings.


#include "Streaming/StreamingSourceProvider.h"

bool UStreamingSourceProvider::GetStreamingSources(TArray<FWorldPartitionStreamingSource>& OutSources) const
{
	for (int32 Index = 0; Index < Locations.Num(); ++Index)
	{
		OutSources.Emplace(
			FName(*FString::Printf(TEXT("StreamSource%d"), Index)),
			Locations[Index],
			FRotator::ZeroRotator,
			EStreamingSourceTargetState::Activated,
			false,
			EStreamingSourcePriority::Default,
			false
		);
	}
	return true;
}

void UStreamingSourceProvider::SetLocation(const FVector& Location)
{
	TArray<FVector> NewLocations;
	NewLocations.AddUnique(Location);
	Locations = Locations;
}

void UStreamingSourceProvider::SetLocations(const TArray<FVector>& InLocations)
{
	Locations = InLocations;
}

void UStreamingSourceProvider::AddLocation(const FVector& Location)
{
	Locations.Add(Location);
}

void UStreamingSourceProvider::ClearLocations()
{
	Locations.Reset();
}