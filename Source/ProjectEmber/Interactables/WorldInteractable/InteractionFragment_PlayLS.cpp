// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/InteractionFragment_PlayLS.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"


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
		SequenceActor->SetActorTransform(Target->GetActorTransform());
		SequencePlayer->Play();

		// 완료 이벤트 바인딩
		SequencePlayer->OnFinished.AddDynamic(this, &UInteractionFragment_PlayLS::OnSequenceFinished);
	}
}

void UInteractionFragment_PlayLS::OnSequenceFinished()
{
	SequencePlayer = nullptr;
	SequenceActor = nullptr;
	OnSequenceFinishedEvent.Broadcast();
}
