// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/LoopEventDirector.h"

#include "GameLoopManagerSubsystem.h"
#include "Data/LoopEventDataAsset.h"
#include "Data/LoopEventSettings.h"
#include "LoopAction/LoopActionBase.h"

ULoopEventDirector::ULoopEventDirector()
{
	LoadAllEvents();
}

void ULoopEventDirector::LoadAllEvents()
{
	const ULoopEventSettings* Setting = GetDefault<ULoopEventSettings>();
	if (!Setting) return;

	DefaultEventID = Setting->DefaultLoopID;
	
	for (const auto& Pair : Setting->LoopDatas)
	{
		int32 LoopID = Pair.Key;

		ULoopEventDataAsset* DataAsset = IsValid(Pair.Value.Get())
			? Pair.Value.Get()
			: Pair.Value.LoadSynchronous();

		if (!DataAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load LoopEventDataAsset for LoopID: %d"), LoopID);
			continue;
		}

		LoadedEvents.Add(LoopID, DataAsset);
	}
}

void ULoopEventDirector::RunStartActions(UGameLoopManagerSubsystem* InManager, int32 LoopID)
{
	CachedLoopManager = InManager;
	CurrentPhase = ELoopEventPhase::Start;
	RunActionsForLoop(LoopID);
}

void ULoopEventDirector::RunMidActions(UGameLoopManagerSubsystem* InManager, int32 LoopID)
{
	CachedLoopManager = InManager;
	CurrentPhase = ELoopEventPhase::Mid;
	RunActionsForLoop(LoopID);
}

void ULoopEventDirector::RunEndActions(UGameLoopManagerSubsystem* InManager, int32 LoopID)
{
	CachedLoopManager = InManager;
	CurrentPhase = ELoopEventPhase::End;
	RunActionsForLoop(LoopID);
}

void ULoopEventDirector::RunActionsForLoop(int32 LoopID)
{
	const TObjectPtr<ULoopEventDataAsset>* DataAssetPtr = LoadedEvents.Find(LoopID);
	
	if (!DataAssetPtr)
	{
		DataAssetPtr = LoadedEvents.Find(DefaultEventID);
	}

	if (!DataAssetPtr || !*DataAssetPtr) return;

	TArray<TObjectPtr<ULoopActionDefinition>> Definitions;
	
	switch (CurrentPhase)
	{
	case ELoopEventPhase::Start:
		Definitions = (*DataAssetPtr)->StartActions;
		break;
	case ELoopEventPhase::Mid:
		Definitions = (*DataAssetPtr)->MidActions;
		break;
	case ELoopEventPhase::End:
		Definitions = (*DataAssetPtr)->EndActions;
		break;
	}
	
	ExecuteActionDefinitions(Definitions);
}

void ULoopEventDirector::ExecuteActionDefinitions(const TArray<TObjectPtr<ULoopActionDefinition>>& Definitions)
{
	PendingActions.Empty();
	
	if (Definitions.Num() == 0)
	{
		if (CachedLoopManager)
		{
			switch (CurrentPhase)
			{
			case ELoopEventPhase::Start:
				CachedLoopManager->NotifyLoopStartReady();
				break;
			case ELoopEventPhase::Mid:
				CachedLoopManager->NotifyLoopMidReady();
				break;
			case ELoopEventPhase::End:
				CachedLoopManager->NotifyLoopEndReady();
				break;
			}
		}
		return;
	}

	FinishedCount = 0;
	
	for (ULoopActionDefinition* Def : Definitions)
	{
		if (!Def->ActionClass) continue;

		ULoopActionBase* NewAction = NewObject<ULoopActionBase>(this, Def->ActionClass);
		if (!NewAction) continue;

		NewAction->Initialize(this, Def);
		PendingActions.Add(NewAction);
		NewAction->ActivateAction();
	}
}


void ULoopEventDirector::OnActionFinished(ULoopActionBase* FinishedAction)
{
	FinishedCount++;

	if (PendingActions.Num() == FinishedCount)
	{
		PendingActions.Empty();
		if (CachedLoopManager)
		{
			switch (CurrentPhase)
			{
				case ELoopEventPhase::Start:
					CachedLoopManager->NotifyLoopStartReady();
				break;
				case ELoopEventPhase::Mid:
					CachedLoopManager->NotifyLoopMidReady();
				break;
				case ELoopEventPhase::End:
					CachedLoopManager->NotifyLoopEndReady();
				break;
			}
		}
	}
}

UWorld* ULoopEventDirector::GetWorldContext() const
{
	return GetWorld();
}
