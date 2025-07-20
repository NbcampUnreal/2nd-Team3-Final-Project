// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/GameLoopManagerSubsystem.h"

#include "LoopEventDirector.h"

void UGameLoopManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// loopAction에서 월드 사용하기 위해 Outer 설정 필수임.
	LoopDirector = NewObject<ULoopEventDirector>(this); 
}

void UGameLoopManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UGameLoopManagerSubsystem::AdvanceLoop()
{
	if (LoopDirector)
	{
		LoopDirector->RunEndActions(this, CurrentLoopID);
	}
}


void UGameLoopManagerSubsystem::NotifyLoopEndReady()
{
	// 디렉터만 호출함. 추후에 추가될 작업이 있을수도?
	OnAllEndLoopReady();
}

void UGameLoopManagerSubsystem::NotifyLoopStartReady()
{
	// 디렉터만 호출함. 추후에 추가될 작업이 있을수도?
	OnAllStartLoopReady();
}

void UGameLoopManagerSubsystem::OnAllEndLoopReady()
{
	++CurrentLoopID;
	
	if (LoopDirector)
	{
		LoopDirector->RunStartActions(this, CurrentLoopID);
	}
}

void UGameLoopManagerSubsystem::OnAllStartLoopReady()
{
	// 아직 없음
}