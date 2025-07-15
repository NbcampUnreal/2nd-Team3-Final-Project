// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/ActorResetManagerSubsystem.h"

#include "GameLoopManagerSubsystem.h"
#include "LoopResettable.h"

void UActorResetManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameLoopManagerSubsystem* LoopManagerSubsystem = GetGameInstance()->GetSubsystem<UGameLoopManagerSubsystem>())
	{
		LoopManagerSubsystem->OnLoopAdvanced.AddDynamic(this, &UActorResetManagerSubsystem::LoopStarted);
	}
}

void UActorResetManagerSubsystem::RegisterResetRequest(AActor* Actor)
{
	if (Actor->GetClass()->ImplementsInterface(ULoopResettable::StaticClass()))
	{
		const FGuid Id = ILoopResettable::Execute_GetID(Actor);
		if (Id.IsValid())
		{
			PendingResetActors.Add(Id, CurrentLoop);
		}
	}
}

bool UActorResetManagerSubsystem::ShouldResetActor(AActor* Actor)
{
	if (Actor->GetClass()->ImplementsInterface(ULoopResettable::StaticClass()))
	{
		const FGuid Id = ILoopResettable::Execute_GetID(Actor);
		return PendingResetActors.Contains(Id);
	}
	return false;
}

void UActorResetManagerSubsystem::ConsumeResetFlag(AActor* Actor)
{
	if (Actor->GetClass()->ImplementsInterface(ULoopResettable::StaticClass()))
	{
		const FGuid Id = ILoopResettable::Execute_GetID(Actor);
		PendingResetActors.Remove(Id);
	}
}
void UActorResetManagerSubsystem::LoopStarted(int32 LoopIndex)
{
	CurrentLoop = LoopIndex;
}
