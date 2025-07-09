//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#include "EMSAsyncCheck.h"
#include "EMSObject.h"

UEMSAsyncCheck::UEMSAsyncCheck(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Type(ESaveFileCheckType::CheckForGame)
	, bCheckSuccess(false)
	, bCheckGameVersion(false) {}

UEMSAsyncCheck* UEMSAsyncCheck::CheckSaveFiles(UObject* WorldContextObject, ESaveFileCheckType CheckType, FString CustomSaveName, bool bComplexCheck)
{
	if (UEMSObject* EMSObject = UEMSObject::Get(WorldContextObject))
	{
		if (!EMSObject->IsAsyncSaveOrLoadTaskActive())
		{
			UEMSAsyncCheck* CheckTask = NewObject<UEMSAsyncCheck>(GetTransientPackage());
			CheckTask->EMS = EMSObject;
			CheckTask->Type = CheckType;
			CheckTask->bCheckGameVersion = bComplexCheck;
			CheckTask->SaveFileName = CustomSaveName;
			return CheckTask;
		}
	}

	return nullptr;
}

void UEMSAsyncCheck::Activate()
{
	if (EMS)
	{
		UE_LOG(LogEasyMultiSave, Log, TEXT("Current Package Version: %d"), GPackageFileUEVersion.ToValue());
		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncCheck::StartCheck);
	}
}

void UEMSAsyncCheck::StartCheck()
{
	switch (Type)
	{
	case ESaveFileCheckType::CheckForCustom:
		CheckCustom();
		break;

	case ESaveFileCheckType::CheckForCustomSlot:
		CheckCustomSlot();
		break;

	default:
		bCheckSuccess = EMS->CheckSaveGameIntegrity(EDataLoadType::DATA_Object, EMS->SlotInfoSaveFile(), bCheckGameVersion);

		if (Type == ESaveFileCheckType::CheckForSlotOnly)
		{
			EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncCheck::CompleteCheck);
		}
		else
		{
			EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncCheck::CheckPlayer);
		}
		break;
	}
}

void UEMSAsyncCheck::CheckPlayer()
{
	bCheckSuccess = EMS->CheckSaveGameIntegrity(EDataLoadType::DATA_Player, EMS->PlayerSaveFile(), bCheckGameVersion);
	EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncCheck::CheckLevel);
}

void UEMSAsyncCheck::CheckLevel()
{
	bCheckSuccess = EMS->CheckSaveGameIntegrity(EDataLoadType::DATA_Level, EMS->ActorSaveFile(), bCheckGameVersion);
	EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncCheck::CompleteCheck);
}

void UEMSAsyncCheck::CheckCustom()
{
	bCheckSuccess = EMS->CheckSaveGameIntegrity(EDataLoadType::DATA_Object, EMS->CustomSaveFile(SaveFileName, FString()), bCheckGameVersion);
	EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncCheck::CompleteCheck);
}

void UEMSAsyncCheck::CheckCustomSlot()
{
	bCheckSuccess = EMS->CheckSaveGameIntegrity(EDataLoadType::DATA_Object, EMS->CustomSaveFile(SaveFileName, EMS->GetCurrentSaveGameName()), bCheckGameVersion);
	EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncCheck::CompleteCheck);
}

void UEMSAsyncCheck::CompleteCheck()
{
	SetReadyToDestroy();

	if (bCheckSuccess)
	{
		OnCompleted.Broadcast();
		return;
	}

	OnFailed.Broadcast();
}
