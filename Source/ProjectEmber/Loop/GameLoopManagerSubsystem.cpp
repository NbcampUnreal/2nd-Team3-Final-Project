// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/GameLoopManagerSubsystem.h"

void UGameLoopManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UGameLoopManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UGameLoopManagerSubsystem::AdvanceLoop()
{
	EndLoop();

	++CurrentLoopID;

	StartLoop();
}

void UGameLoopManagerSubsystem::EndLoop()
{
	OnLoopEnding.Broadcast();
}

void UGameLoopManagerSubsystem::StartLoop()
{
	OnLoopAdvanced.Broadcast(CurrentLoopID);
}