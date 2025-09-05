// Fill out your copyright notice in the Description page of Project Settings.


#include "Streaming/StreamingSourceHandle.h"

#include "StreamingSourceProvider.h"
#include "WorldPartition/WorldPartitionSubsystem.h"

void UStreamingSourceHandle::Initialize(const TArray<FVector>& InLocations)
{
	Locations = InLocations;
}

void UStreamingSourceHandle::BeginWait()
{
	Subsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>();

	if (!StreamingSourceProvider)
	{
		StreamingSourceProvider = NewObject<UStreamingSourceProvider>(this);
	}

	StreamingSourceProvider->SetLocations(Locations);
	Subsystem->RegisterStreamingSourceProvider(StreamingSourceProvider);

	OnCheckStreaming();
}

void UStreamingSourceHandle::Cleanup()
{
	if (StreamingSourceProvider && Subsystem)
	{
		Subsystem->UnregisterStreamingSourceProvider(StreamingSourceProvider);
		StreamingSourceProvider->ClearLocations();
	}
}

void UStreamingSourceHandle::OnCheckStreaming()
{
	if (Subsystem->IsStreamingCompleted(StreamingSourceProvider))
	{
		OnStreamingCompleted.Broadcast();
	}
	else
	{
		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &UStreamingSourceHandle::OnCheckStreaming);
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDel);
	}
}
