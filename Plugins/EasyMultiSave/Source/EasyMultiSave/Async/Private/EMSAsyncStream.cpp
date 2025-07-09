//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  


#include "EMSAsyncStream.h"
#include "EMSObject.h"
#include "Engine/Engine.h"
#include "Async/Async.h"

/**
Init
**/

UEMSAsyncStream::UEMSAsyncStream(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Mode(ESaveGameMode::MODE_All)
	, PrunedData(FMultiLevelStreamingData())
	, bIsActive(false)
	, TotalActors(0)
	, LoadedActorNum(0)
	, LoadedTotalNum(0)
	, FailSafeLoopNum(0) {}

bool UEMSAsyncStream::InitStreamingLoadTask(UEMSObject* EMSObject, ULevel* InLevel)
{
	//Check to see if an instance with the same streaming level is active.
	if (FAsyncSaveHelpers::IsStreamAutoLoadActive(InLevel))
	{
		UE_LOG(LogEasyMultiSave, Log, TEXT("Skipped loading streaming level while same async task is already active"));
		return false;
	}

	UEMSAsyncStream* LoadTask = NewObject<UEMSAsyncStream>(GetTransientPackage());
	if (LoadTask)
	{
		LoadTask->StreamingLevel = InLevel;
		LoadTask->EMS = EMSObject;
		LoadTask->RegisterWithGameInstance(EMSObject);
		LoadTask->Activate();
		return true;
	}

	return false;
}

void UEMSAsyncStream::Activate()
{
	if (EMS)
	{
		bIsActive = true;
		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncStream::StartLoad);
	}
}

void UEMSAsyncStream::StartLoad()
{
	if (EMS)
	{
		if (!SetupLevelActors())
		{
			Deactivate();
			return;
		}

		if (FSettingHelpers::IsDeferredLoading())
		{
			EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncStream::DeferredLoadStreamActors);
			return;
		}

		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncStream::ScheduleStreamingLoad);
	}
}

bool UEMSAsyncStream::SetupLevelActors()
{
	if (!EMS || !StreamingLevel || EMS::ArrayEmpty(StreamingLevel->Actors))
	{
		return false;
	}

	const int32 EstimatedSize = StreamingLevel->Actors.Num();
	StreamActors.Reserve(EstimatedSize);
	StreamActorsMap.Reserve(EstimatedSize);

	//This is somewhat like the Prepare Actors function
	for (AActor* Actor : StreamingLevel->Actors)
	{
		if (EMS->IsValidActor(Actor) 
			&& FActorHelpers::IsPlacedActor(Actor) 
			&& !FActorHelpers::IsLoaded(Actor) 
			&& !FActorHelpers::IsSkipSave(Actor))
		{
			StreamActors.Add(Actor);

			const FName ActorName(*FActorHelpers::GetFullActorName(Actor));
			StreamActorsMap.Add(ActorName, Actor);
		}
	}

	if (EMS::ArrayEmpty(StreamActors))
	{
		return false;
	}

	//Prune saved Actor data to prevent looking up insanely huge arrays
	TArray<FActorSaveData> PrunedActors = EMS->GetMultiLevelStreamData().ActorArray;
	FSaveHelpers::PruneSavedActors(StreamActorsMap, PrunedActors);
	PrunedData.CopyActors(PrunedActors);

	if (!PrunedData.HasLevelActors())
	{
		return false;
	}

	//Distance based loading. The sorting must be inverted here, so we sort the Actors, not the data.
	if (const APlayerController* PC = EMS->GetPlayerController())
	{
		StreamActors.Sort([PC](const TWeakObjectPtr<AActor>& AWeak, const TWeakObjectPtr<AActor>& BWeak)
		{
			const AActor* A = AWeak.Get();
			const AActor* B = BWeak.Get();

			if (!A || !B)
			{
				return false;
			}

			return FActorHelpers::CompareDistance(A->GetActorLocation(), B->GetActorLocation(), PC);
		});
	}

	TotalActors = StreamActors.Num();
	EMS->AllocateRealLoadedActors(TotalActors);

	return true;
}

void UEMSAsyncStream::LoadActor(TWeakObjectPtr<AActor> ActorWeakPtr)
{
	if (!bIsActive)
	{
		Deactivate();
		return;
	}

	AActor* Actor = ActorWeakPtr.Get();
	if (!Actor)
	{
		return;
	}

	//Use TMap lookup. Faster on Game-Thread
	const FName ActorKey(FActorHelpers::GetFullActorName(Actor));
	const FActorSaveData* ActorDataPtr = PrunedData.ActorMap.Find(ActorKey);

	if (ActorDataPtr)
	{
		const FActorSaveData ActorData = *ActorDataPtr;
		LoadStreamingActor(Actor, ActorData);
	}
}

void UEMSAsyncStream::LoadActorMultiThread(TWeakObjectPtr<AActor> ActorWeakPtr)
{
	if (!bIsActive)
	{
		Deactivate();
		return;
	}

	AActor* Actor = ActorWeakPtr.Get();
	if (!Actor)
	{
		return;
	}

	//Use TArray lookup on background thread
	for (const FActorSaveData& ActorData : PrunedData.ActorArray)
	{
		if (FSaveHelpers::CompareIdentifiers(ActorData.Name, FActorHelpers::GetFullActorName(Actor)))
		{
			LoadStreamingActor(Actor, ActorData);
			break;
		}
	}
}

void UEMSAsyncStream::LoadStreamingActor(AActor* Actor, const FActorSaveData& ActorData)
{
	if (IsValid(EMS) && IsValid(Actor))
	{
		EMS->LoadStreamingActor(Actor, ActorData);
	}
}

/**
Multi-Thread Loading
**/

void UEMSAsyncStream::ScheduleStreamingLoad()
{
	if (FSettingHelpers::IsMultiThreadLoading())
	{
		AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [this]()
		{
			for (TWeakObjectPtr<AActor> ActorWeakPtr : StreamActors)
			{
				if (ActorWeakPtr.IsValid())
				{
					LoadActorMultiThread(ActorWeakPtr);
				}
			}

			FinishLoadingStreamLevel();
		});
	}
	else
	{
		for (TWeakObjectPtr<AActor> ActorWeakPtr : StreamActors)
		{
			if (ActorWeakPtr.IsValid())
			{
				LoadActor(ActorWeakPtr);
			}
		}

		FinishLoadingStreamLevel();
	}
}

/**
Deferred Loading
**/

void UEMSAsyncStream::DeferredLoadStreamActors()
{
	if (!EMS || !bIsActive)
	{
		Deactivate();
		return;
	}

	if (TotalActors <= 0)
	{
		FinishLoadingStreamLevel();
		return;
	}

	const uint32 BatchSize = FSettingHelpers::GetLoadBatchSize();
	uint32 LoadedThisTick = 0;

	while (LoadedThisTick < BatchSize && LoadedTotalNum < TotalActors)
	{
		//Failsafe due to the volatile nature of WP
		if (++FailSafeLoopNum >= EMS::BigNumber)
		{
			FinishLoadingStreamLevel();
			return;
		}

		//Grab the next actor in the list
		TWeakObjectPtr<AActor> Actor = StreamActors[LoadedTotalNum];
		if (Actor.IsValid())
		{
			LoadActor(Actor);
			++LoadedThisTick;
		}

		++LoadedTotalNum;
	}

	//If we've consumed the entire list, finish up
	if (LoadedTotalNum >= TotalActors)
	{
		FinishLoadingStreamLevel();
	}
	else
	{
		EMS->GetTimerManager().SetTimerForNextTick(this,&UEMSAsyncStream::DeferredLoadStreamActors);
	}
}

/**
Finish
**/

void UEMSAsyncStream::FinishLoadingStreamLevel()
{
	UE_LOG(LogEasyMultiSave, Log, TEXT("Loaded %d World Partition Actors"), TotalActors);
	EndTask(true);
}

void UEMSAsyncStream::ForceDestroy()
{
	if (EMS)
	{
		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncStream::Deactivate);
	}
	else
	{
		Deactivate();
	}
}

void UEMSAsyncStream::Deactivate()
{
	EndTask(false);
}

void UEMSAsyncStream::EndTask(const bool bBroadcastFinish)
{
	auto SetInactive = [this, bBroadcastFinish]()
	{
		if (bBroadcastFinish && EMS)
		{
			EMS->BroadcastOnPartitionLoaded();
		}

		bIsActive = false;
		SetReadyToDestroy();
	};

	//Deactivate on Game thread where it was spawned
	if (!IsInGameThread())
	{
		AsyncTask(ENamedThreads::GameThread, [SetInactive]()
		{
			SetInactive();
		});
	}
	else
	{
		SetInactive();
	}
}

