// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/InteractionFragment_PlayLS.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Kismet/GameplayStatics.h"


UInteractionFragment_PlayLS::UInteractionFragment_PlayLS()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionFragment_PlayLS::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);

	if (!LevelSequence || IsValid(SequencePlayer) && SequencePlayer->IsPlaying())
	{
		return;
	}

	AActor* Target = SequenceTargetActor.Get() != nullptr ? SequenceTargetActor.Get() : GetOwner();

	if (!Target) return;

	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = false;
	Settings.bDisableCameraCuts = false;

	ALevelSequenceActor* RawActor = nullptr;
	
	SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		LevelSequence,
		Settings,
		RawActor
		);
	
	SequenceActor = RawActor;
	

	if (SequencePlayer && SequenceActor)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			PC->bAutoManageActiveCameraTarget = false;
		}
		
		SequenceActor->SetActorTransform(Target->GetActorTransform());
		SequencePlayer->Play();

		// 완료 이벤트 바인딩
		SequencePlayer->OnFinished.AddDynamic(this, &UInteractionFragment_PlayLS::OnSequenceFinished);
	}
}

void UInteractionFragment_PlayLS::OnSequenceFinished()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bAutoManageActiveCameraTarget = true; // 복구
		PC->SetViewTargetWithBlend(PC->GetPawn(), 0.3f); // 플레이어로 복귀
	}
	
	SequencePlayer = nullptr;
	SequenceActor = nullptr;
	OnSequenceFinishedEvent.Broadcast();
}
