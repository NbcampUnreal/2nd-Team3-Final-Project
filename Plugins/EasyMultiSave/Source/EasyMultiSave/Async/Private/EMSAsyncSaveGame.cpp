//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#include "EMSAsyncSaveGame.h"
#include "EMSFunctionLibrary.h"
#include "Modules/ModuleManager.h"
#include "Engine/Engine.h"
#include "Async/Async.h"

/**
Init
**/

UEMSAsyncSaveGame::UEMSAsyncSaveGame(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Mode(ESaveGameMode::MODE_All)
	, Data(0)
	, bIsActive(false)
	, bFinishedStep(false)
	, bHasFailed(false)
	, bAutoSaveLevel(false)
	, bMemoryOnly (false) {}

UEMSAsyncSaveGame* UEMSAsyncSaveGame::AsyncSaveActors(UObject* WorldContextObject, int32 Data)
{
	if (UEMSObject* EMSObject = UEMSObject::Get(WorldContextObject))
	{
		const ESaveGameMode Mode = FAsyncSaveHelpers::GetMode(Data);

		if (EMSObject->IsAsyncSaveOrLoadTaskActive(Mode))
		{
			return nullptr;
		}

		UEMSAsyncSaveGame* SaveTask = NewObject<UEMSAsyncSaveGame>(GetTransientPackage());
		if (SaveTask)
		{
			SaveTask->EMS = EMSObject;
			SaveTask->Data = Data;
			SaveTask->Mode = Mode;
			return SaveTask;
		}	
	}

	return nullptr;
}

void UEMSAsyncSaveGame::AutoSaveLevelActors(UEMSObject* EMSObject)
{
	UEMSAsyncSaveGame* SaveTask = NewObject<UEMSAsyncSaveGame>(GetTransientPackage());
	if (SaveTask)
	{
		SaveTask->EMS = EMSObject;
		SaveTask->Data = ENUM_TO_FLAG(ESaveTypeFlags::SF_Level);
		SaveTask->Mode = ESaveGameMode::MODE_Level;
		SaveTask->bAutoSaveLevel = true;
		SaveTask->bMemoryOnly = FSettingHelpers::IsMemoryOnlySave();
		SaveTask->RegisterWithGameInstance(EMSObject);
		SaveTask->Activate();
	}
}

void UEMSAsyncSaveGame::Activate()
{
	if (EMS)
	{
		//Warn when still streaming
		if (!bAutoSaveLevel && Data & ENUM_TO_FLAG(ESaveTypeFlags::SF_Level))
		{
			if (EMS->IsLevelStreaming())
			{
				UE_LOG(LogEasyMultiSave, Warning, TEXT("Async save attempt while streaming. To avoid possible data loss, use 'Is Level Streaming Active' to check."));
			}
		}

		bIsActive = true;

		//For World Partition auto-saving, we only prepare runtime Actors, the preparation process can be skipped for placed Actors in cells
		const EPrepareType PrepareType = bAutoSaveLevel ? EPrepareType::PT_RuntimeOnly : EPrepareType::PT_Default;
		EMS->PrepareLoadAndSaveActors(Data, EAsyncCheckType::CT_Save, PrepareType);

		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncSaveGame::StartSaving);
	}
}

void UEMSAsyncSaveGame::StartSaving()
{
	bHasFailed = false;

	if (EMS)
	{
		const bool bSaveToMemory = bAutoSaveLevel && bMemoryOnly;
		if (!bSaveToMemory)
		{
			//Save current slot
			EMS->SaveSlotInfoObject(EMS->GetCurrentSaveGameName());
		}

		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncSaveGame::SavePlayer);
	}
}

/**
Player
**/

void UEMSAsyncSaveGame::SavePlayer()
{
	bFinishedStep = false;

	if (EMS)
	{
		if (FSettingHelpers::IsMultiThreadSaving())
		{
			AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [this]()
			{
				InternalSavePlayer();
			});
		}
		else
		{
			InternalSavePlayer();
		}

		TryMoveToNextStep(ENextStepType::SaveLevel);
	}
}

void UEMSAsyncSaveGame::InternalSavePlayer()
{
	if (Data & ENUM_TO_FLAG(ESaveTypeFlags::SF_Player))
	{
		if (!EMS->SavePlayerActors(EMS->GetPlayerController(), EMS->PlayerSaveFile()))
		{
			bHasFailed = true;
		}
	}

	bFinishedStep = true;
}

/**
Level
**/

void UEMSAsyncSaveGame::SaveLevel()
{
	bFinishedStep = false;

	if (EMS)
	{
		if (FSettingHelpers::IsMultiThreadSaving())
		{
			AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [this]()
			{
				InternalSaveLevel();
			});
		}
		else
		{
			InternalSaveLevel();
		}

		TryMoveToNextStep(ENextStepType::FinishSave);
	}
}

void UEMSAsyncSaveGame::InternalSaveLevel()
{
	if (Data & ENUM_TO_FLAG(ESaveTypeFlags::SF_Level))
	{
		const bool bPrevHasFailed = bHasFailed;

		if(EMS->SaveLevelActors(bMemoryOnly))
		{
			bHasFailed = false;
		}
		else
		{
			bHasFailed = bPrevHasFailed;
		}
	}

	bFinishedStep = true;
}

/**
Finish
**/

void UEMSAsyncSaveGame::FinishSaving()
{
	if (EMS)
	{
		bIsActive = false;

		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncSaveGame::CompleteSavingTask);
	}
}

void UEMSAsyncSaveGame::CompleteSavingTask()
{
	if (bHasFailed)
	{
		OnFailed.Broadcast();
	}
	else
	{
		OnCompleted.Broadcast();
	}

	SetReadyToDestroy();
}

/**
Helper Functions
**/

void UEMSAsyncSaveGame::TryMoveToNextStep(ENextStepType Step)
{
	//This is used to delay further execution until multi-thread code finished, but without blocking.

	if (EMS)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this, Step]()
		{
			if (bFinishedStep)
			{
				if (Step == ENextStepType::FinishSave)
				{
					EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncSaveGame::FinishSaving);
				}
				else
				{
					EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncSaveGame::SaveLevel);
				}
			}
			else
			{
				TryMoveToNextStep(Step);
			}
		});

		EMS->GetTimerManager().SetTimerForNextTick(TimerDelegate);
	}
}