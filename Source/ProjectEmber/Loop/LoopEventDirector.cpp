// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/LoopEventDirector.h"

#include "GameLoopManagerSubsystem.h"
#include "Data/LoopEventDataAsset.h"
#include "Data/LoopEventSettings.h"
#include "LoopAction/LoopActionBase.h"

void ULoopEventDirector::RunStartActions(UGameLoopManagerSubsystem* InManager, int32 LoopID)
{
	CachedLoopManager = InManager;
	bIsStart = true;
	RunActionsForLoop(LoopID);
}

void ULoopEventDirector::RunEndActions(UGameLoopManagerSubsystem* InManager, int32 LoopID)
{
	CachedLoopManager = InManager;
	bIsStart = false;
	RunActionsForLoop(LoopID);
}

void ULoopEventDirector::RunActionsForLoop(int32 LoopID)
{
	const ULoopEventSettings* Setting = GetDefault<ULoopEventSettings>();
	if (!Setting) return;
	
	const TObjectPtr<ULoopEventDataAsset>* DataAssetPtr = Setting->LoopEventMap.Find(LoopID);

	// 루프 ID가 없으면 기본값으로 대체
	if (!DataAssetPtr)
	{
		DataAssetPtr = Setting->LoopEventMap.Find(Setting->DefaultLoopID);
	}

	if (!DataAssetPtr || !*DataAssetPtr) return;

	const TArray<TObjectPtr<ULoopActionDefinition>>& Definitions = bIsStart 
		? (*DataAssetPtr)->StartActions 
		: (*DataAssetPtr)->EndActions;

	ExecuteActionDefinitions(Definitions);
}

void ULoopEventDirector::ExecuteActionDefinitions(const TArray<TObjectPtr<ULoopActionDefinition>>& Definitions)
{
	PendingActions.Empty();
	
	if (Definitions.Num() == 0)
	{
		if (CachedLoopManager)
		{
			if (bIsStart) CachedLoopManager->NotifyLoopStartReady();
			else CachedLoopManager->NotifyLoopEndReady();
		}
		return;
	}
	
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
	PendingActions.Remove(FinishedAction);

	if (PendingActions.Num() == 0)
	{
		if (CachedLoopManager)
		{
			if (bIsStart) CachedLoopManager->NotifyLoopStartReady();
			else CachedLoopManager->NotifyLoopEndReady();
		}
	}
}

UWorld* ULoopEventDirector::GetWorldContext() const
{
	return GetWorld();
}