// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/LoopAction/LoopAction_UpdateResetManager.h"

#include "Loop/ActorResetManagerSubsystem.h"
#include "Loop/GameLoopManagerSubsystem.h"
#include "Loop/LoopEventDirector.h"

void ULoopAction_UpdateResetManager::ActivateAction_Implementation()
{
	Super::ActivateAction_Implementation();

	if (!OwnerDirector) return;

	// 월드 얻기
	UWorld* World = OwnerDirector->GetWorld();
	if (!World) return;

	// 루프 매니저 얻기
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return;

	UActorResetManagerSubsystem* ResetManager = GameInstance->GetSubsystem<UActorResetManagerSubsystem>();
	UGameLoopManagerSubsystem* LoopManager = GameInstance->GetSubsystem<UGameLoopManagerSubsystem>();

	if (ResetManager && LoopManager)
	{
		const int32 CurrentLoopID = LoopManager->GetCurrentLoopID();
		ResetManager->SetCurrentLoopID(CurrentLoopID);
	}

	FinishAction();
}
