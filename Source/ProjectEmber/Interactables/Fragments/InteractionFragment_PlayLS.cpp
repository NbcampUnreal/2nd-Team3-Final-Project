// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Fragments/InteractionFragment_PlayLS.h"

#include <memory>

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Streaming/StreamingSourceProvider.h"
#include "WorldPartition/WorldPartitionSubsystem.h"


UInteractionFragment_PlayLS::UInteractionFragment_PlayLS()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionFragment_PlayLS::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);

	if (!LevelSequenceActor || !LevelSequenceActor->GetSequencePlayer())
	{
		return;
	}

	// 시퀀스 플레이어 가져오기
	ULevelSequencePlayer* Player = LevelSequenceActor->GetSequencePlayer();
	if (Player->IsPlaying())
	{
		return;
	}
	
	UWorldPartitionSubsystem* Subsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>();

	if (!StreamingSourceProvider)
	{
		StreamingSourceProvider = NewObject<UStreamingSourceProvider>(this);
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
	
	if (!PlayerPawn)
	{
		return;
	}

	StreamingSourceProvider->ClearLocations();
	StreamingSourceProvider->SetLocations({ PlayerPawn->GetActorLocation(), LevelSequenceActor->GetActorLocation() });
	// 등록
	Subsystem->RegisterStreamingSourceProvider(StreamingSourceProvider);

	// 로딩 완료 후 시퀀스 재생
	WaitForStreamingAndPlaySequence(Subsystem);
	
}

void UInteractionFragment_PlayLS::WaitForStreamingAndPlaySequence(UWorldPartitionSubsystem* Subsystem)
{
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, Subsystem]()
	{
		const bool bIsCompleted = Subsystem->IsStreamingCompleted(StreamingSourceProvider);

		OnStreamingStatusUpdated.Broadcast(bIsCompleted);

		if (bIsCompleted)
		{
			StartSequencePlayback();
		}
		else
		{
			WaitForStreamingAndPlaySequence(Subsystem);
		}
	});

	// 한 프레임 후 체크
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDel);
}

void UInteractionFragment_PlayLS::StartSequencePlayback()
{
	ULevelSequencePlayer* Player = LevelSequenceActor->GetSequencePlayer();
	if (!Player)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bAutoManageActiveCameraTarget = false;
	}

	Player->Play();

	// 완료 이벤트
	Player->OnFinished.AddDynamic(this, &UInteractionFragment_PlayLS::OnSequenceFinished);
}

void UInteractionFragment_PlayLS::OnSequenceFinished()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bAutoManageActiveCameraTarget = true;
		PC->SetViewTargetWithBlend(PC->GetPawn(), 0.3f);
	}

	//// 해제
	if (StreamingSourceProvider)
	{
		if (UWorldPartitionSubsystem* Subsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>())
		{
			Subsystem->UnregisterStreamingSourceProvider(StreamingSourceProvider);
		}
		StreamingSourceProvider->ClearLocations();
	}

	OnSequenceFinishedEvent.Broadcast();
}
