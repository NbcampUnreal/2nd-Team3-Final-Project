// Fill out your copyright notice in the Description page of Project Settings.

#include "MiniGameInteractableActor.h"
#include "MiniGameCondition.h"
#include "EmberLog/EmberLog.h"
#include "Quest/Condition/QuestCondition.h"

void AMiniGameInteractableActor::Interact_Implementation(AActor* Interactor)
{
	if (bIsPlaying) return;
	// 중복 실행 방지 코드 필요
	Super::Interact_Implementation(Interactor);
	// Interactor 저장 필요?
	BeginMiniGame();
}

void AMiniGameInteractableActor::EndInteract_Implementation()
{
	Super::EndInteract_Implementation();

	EndMiniGame();
}

void AMiniGameInteractableActor::BeginMiniGame()
{
	for (UMiniGameFragment* Fragment : MiniGameFragments)
	{
		if (Fragment)
		{
			Fragment->OnMiniGameStart(this);
		}
	}
	
	if (UGameplayEventSubsystem* EventSubsystem = UGameplayEventSubsystem::GetGameplayEvent(GetWorld()))
	{
		if (!EventSubsystem->OnGameEvent.IsAlreadyBound(this, &AMiniGameInteractableActor::CheckConditionCompletion))
		{
			EventSubsystem->OnGameEvent.AddDynamic(this, &AMiniGameInteractableActor::CheckConditionCompletion);
		}
	}
	bIsPlaying = true;
}

void AMiniGameInteractableActor::EndMiniGame()
{	
	for (UMiniGameFragment* Fragment : MiniGameFragments)
	{
		if (Fragment)
		{
			Fragment->OnMiniGameEnd();
		}
	}

	if (bAllConditionsCleared)
	{
		UE_LOG(LogEmberInteraction, Error, TEXT("MiniGameCleared!"));
		//보상 지급 + 미니게임 비활성화
	}

	bIsPlaying = false;
}

//미니 게임 시작 시 이벤트 등록
void AMiniGameInteractableActor::CheckConditionCompletion(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	for (UMiniGameCondition* Condition : MiniGameConditions)
	{
		if (Condition && Condition->EventTag == EventTag)
		{
			Condition->OnEvent(EventTag, EventData);
		}
	}

	for (const UMiniGameCondition* Condition : MiniGameConditions)
	{
		if (!Condition || !Condition->IsFulfilled())
		{
			return;
		}
	}
	
	bAllConditionsCleared = true;
	
	// 조건 만족 후 클리어
	if (Owner && Owner->Implements<UInteractable>())
	{
		IInteractable::Execute_EndInteract(Owner);
	}
}
