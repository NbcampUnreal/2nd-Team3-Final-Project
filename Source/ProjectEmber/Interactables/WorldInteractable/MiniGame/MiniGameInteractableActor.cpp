// Fill out your copyright notice in the Description page of Project Settings.

#include "MiniGameInteractableActor.h"
#include "EmberLog/EmberLog.h"
#include "Interactables/WorldInteractable/InteractionReceiverComponent.h"
#include "Quest/Condition/QuestCondition.h"

void AMiniGameInteractableActor::Interact_Implementation(AActor* Interactor)
{
	if (bIsPlaying) return;
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
	bIsPlaying = false;
}

//미니 게임 시작 시 이벤트 등록
void AMiniGameInteractableActor::CheckConditionCompletion(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	for (UQuestCondition* Condition : QuestConditions)
	{
		if (Condition && Condition->EventTag == EventTag)
		{
			Condition->OnEvent(EventTag, EventData);
		}
	}

	for (const UQuestCondition* Condition : QuestConditions)
	{
		if (!Condition || !Condition->IsFulfilled())
		{
			return;
		}
	}
	
	ReceiverComponent->bCanBeTriggeredAgain = false;
	
	// 조건 만족 후 클리어
	if (this->Implements<UInteractable>())
	{
		IInteractable::Execute_EndInteract(this);
	}
}
