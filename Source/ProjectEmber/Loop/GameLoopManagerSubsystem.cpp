// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/GameLoopManagerSubsystem.h"

#include "LoopEventDirector.h"
#include "LoopTimeOfDaySubsystem.h"
#include "Kismet/GameplayStatics.h"

void UGameLoopManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// loopAction에서 월드 사용하기 위해 Outer 설정 필수임.
	LoopDirector = NewObject<ULoopEventDirector>(this);

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UGameLoopManagerSubsystem::OnWorldReady);
}

void UGameLoopManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UGameLoopManagerSubsystem::AdvanceLoopEvent()
{
	if (CurrentPhase == ELoopEventPhase::Start) return;
	CurrentPhase = ELoopEventPhase::Start;
	if (LoopDirector)
	{
		LoopDirector->RunStartActions(this, CurrentLoopID);
	}
}

void UGameLoopManagerSubsystem::MidLoopEvent()
{
	if (CurrentPhase == ELoopEventPhase::Mid) return;
	CurrentPhase = ELoopEventPhase::Mid;
	if (LoopDirector)
	{
		LoopDirector->RunMidActions(this, CurrentLoopID);
	}
}

void UGameLoopManagerSubsystem::EndLoopEvent()
{
	if (CurrentPhase == ELoopEventPhase::End) return;
	CurrentPhase = ELoopEventPhase::End;
	if (LoopDirector)
	{
		LoopDirector->RunEndActions(this, CurrentLoopID);
	}
}

void UGameLoopManagerSubsystem::CheckAllEndConditionReady()
{
	if (bAllEndEventFinished && bDayTimeEnded)
	{
		StartFade(true, FLinearColor::Black, FadeDuration);
		GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &UGameLoopManagerSubsystem::OnAllEndLoopReady, FadeDuration, false);
	}
}

void UGameLoopManagerSubsystem::OnDayEnd24()
{
	bDayTimeEnded = true;
	CheckAllEndConditionReady();
}

void UGameLoopManagerSubsystem::NotifyLoopEndReady()
{
	bAllEndEventFinished = true;
	CheckAllEndConditionReady();
}

void UGameLoopManagerSubsystem::NotifyLoopMidReady()
{
	OnAllMidLoopReady();
}

void UGameLoopManagerSubsystem::NotifyLoopStartReady()
{
	StartFade(true, FLinearColor::Black, FadeDuration);
	GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &UGameLoopManagerSubsystem::OnAllStartLoopReady, FadeDuration, false);
}


void UGameLoopManagerSubsystem::OnAllEndLoopReady()
{
	// 시간 정지 후 mid 이벤트로 이동
	if (auto TOD = GetWorld()->GetSubsystem<ULoopTimeOfDaySubsystem>())
	{
		TOD->SetPause(true);
	}
	
	MidLoopEvent();
}

void UGameLoopManagerSubsystem::OnAllMidLoopReady()
{
	++CurrentLoopID;
	
	AdvanceLoopEvent();
	StartFade(false, FLinearColor::Black, FadeDuration);
}

void UGameLoopManagerSubsystem::OnAllStartLoopReady()
{
	if (auto TOD = GetWorld()->GetSubsystem<ULoopTimeOfDaySubsystem>())
	{
		TOD->StartTimeLoop();
	}
}

void UGameLoopManagerSubsystem::OnWorldReady(UWorld* World, const UWorld::InitializationValues IVS)
{
	AdvanceLoopEvent();
}

void UGameLoopManagerSubsystem::StartFade(bool bFadeOut, const FLinearColor& Color, float Duration)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && PC->PlayerCameraManager)
	{
		if (bFadeOut)
		{
			PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, Duration, Color, false, false);
		}
		else
		{
			{
				PC->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, Duration, Color, false, false);
			}
		}
	}
}
