// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/LoopResettableInteractableActor.h"

#include "Condition/InteractionCondition.h"
#include "Loop/ActorResetManagerSubsystem.h"


void ALoopResettableInteractableActor::EndInteract_Implementation()
{
	Super::EndInteract_Implementation();

	auto ResetManager = GetGameInstance()->GetSubsystem<UActorResetManagerSubsystem>();

	if (!ResetManager) return;

	ResetManager->RegisterResetRequest(this);
}

void ALoopResettableInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	if (UActorResetManagerSubsystem* ResetManager = GetGameInstance()->GetSubsystem<UActorResetManagerSubsystem>())
	{
		if (ResetManager->ShouldResetActor(this))
		{
			ILoopResettable::Execute_OnLoopReset(this);
			ResetManager->ConsumeResetFlag(this);
		}
	}
}

void ALoopResettableInteractableActor::OnLoopReset_Implementation()
{
	// 기본적으로 시작 컨디션 리셋만
	for (auto& Condition : StartConditions)
	{
		Condition->Reset();
	}
}

FGuid ALoopResettableInteractableActor::GetID_Implementation() const
{
	return PersistentActorID;
}

void ALoopResettableInteractableActor::GenerateNewGuid()
{
	PersistentActorID = FGuid::NewGuid();
}

//#if WITH_EDITOR
//void ALoopResettableInteractableActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//	if (!PersistentActorID.IsValid())
//	{
//		PersistentActorID = FGuid::NewGuid();
//	}
//}
//#endif