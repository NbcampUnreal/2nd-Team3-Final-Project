//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#include "EMSAsyncWait.h"
#include "EMSObject.h"

UEMSAsyncWait* UEMSAsyncWait::AsyncWaitForOperation(UObject* WorldContextObject, EAsyncCheckType CheckType)
{
	if (UEMSObject* EMSObject = UEMSObject::Get(WorldContextObject))
	{
		UEMSAsyncWait* WaitTask = NewObject<UEMSAsyncWait>(GetTransientPackage());
		WaitTask->EMS = EMSObject;
		WaitTask->bSaveOrLoadTaskWasActive = false;
		WaitTask->Type = CheckType;
		return WaitTask;
	}

	return nullptr;
}

void UEMSAsyncWait::Activate()
{
	if (EMS)
	{
		StartWaitTask();

		const float WaitTime = UEMSPluginSettings::Get()->AsyncWaitDelay;
		if (WaitTime > 0.f)
		{
			EMS->GetTimerManager().SetTimer(WaitTimerHandle, FTimerDelegate::CreateUObject(this, &UEMSAsyncWait::ForceProceed), WaitTime, false);
		}
	}
}

void UEMSAsyncWait::StartWaitTask()
{
	if (!EMS)
	{
		CompleteWaitTask();
		return;
	}

	//Wait as long as the current async operation is completed.
	if (EMS->IsAsyncSaveOrLoadTaskActive(ESaveGameMode::MODE_All, Type, false))
	{
		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncWait::StartWaitTask);
		bSaveOrLoadTaskWasActive = true;
	}
	else
	{
		//Without save data, we will proceed  
		const bool bSaveFileExists = EMS->DoesSaveGameExist(EMS->GetCurrentSaveGameName());
		if (!bSaveFileExists)
		{
			//Wait a few frames, due to the async nature of Save and Load operations
			if (++CheckCounter > EMS::MinAsyncWaitFrames)
			{
				bSaveOrLoadTaskWasActive = true;
			}
		}

		if (bSaveOrLoadTaskWasActive)
		{
			EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncWait::CompleteWaitTask);
		}
		else
		{
			EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncWait::StartWaitTask);
		}
	}	
}

void UEMSAsyncWait::ForceProceed()
{
	bSaveOrLoadTaskWasActive = true;
}

void UEMSAsyncWait::CompleteWaitTask()
{
	if (EMS)
	{
		EMS->GetTimerManager().ClearAllTimersForObject(this);
	}

	OnCompleted.Broadcast();
	SetReadyToDestroy();
}
