// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/WorldInteractableActor.h"

#include "GameplayTagPayload.h"
#include "Condition/InteractionCondition.h"
#include "InteractionFragment.h"
#include "GameplayTag/EmberGameplayTag.h"
#include "MessageBus/MessageBus.h"


AWorldInteractableActor::AWorldInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWorldInteractableActor::HandleEventTag(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	if (!bIsPlaying)
	{
		HandleStartConditionEvent(EventTag, EventData);
	}
	else
	{
		HandleClearConditionEvent(EventTag, EventData);
	}
}

void AWorldInteractableActor::HandleStartConditionEvent(const FGameplayTag& EventTag,
	const FGameplayEventData& EventData)
{
	for (UInteractionCondition* Condition : StartConditions)
	{
		if (Condition)
		{
			Condition->OnEvent(EventTag, EventData);
		}
	}
	
	if (AreAllConditionsMet(StartConditions))
	{
		OnAllActivationConditionsMet.Broadcast(EventData);
	}
}

void AWorldInteractableActor::HandleClearConditionEvent(const FGameplayTag& EventTag,
	const FGameplayEventData& EventData)
{
	for (UInteractionCondition* Condition : ClearConditions)
	{
		if (Condition)
		{
			Condition->OnEvent(EventTag, EventData);
		}
	}
		
	if (AreAllConditionsMet(ClearConditions))
	{
		OnAllClearConditionsMet.Broadcast(EventData);

		// 시작 여부 컨디션에 클리어 태그 넣기 (여긴 좀 더 고민)
		for (UInteractionCondition* StartCondition : StartConditions)
		{
			if (StartCondition)
			{
				StartCondition->OnEvent(EmberInteractionEventTag::Clear, EventData);
			}
		}
		
		// 메시지 버스로 클리어 태그 보내기
		if (UMessageBus* Bus = UMessageBus::GetInstance())
		{
			UGameplayTagPayload* Payload = NewObject<UGameplayTagPayload>();
			Payload->GameplayTag = EmberInteractionEventTag::Clear;
			Payload->ContextObject = this;

			Bus->BroadcastMessage(InteractionMessages::MessageType, Payload);
		}
	}
}

void AWorldInteractableActor::TryInteract_Implementation(AActor* Interactor)
{
	if (CanInteract())
	{
		bIsPlaying = true;
		IInteractable::Execute_Interact(this, Interactor);
	}
}

bool AWorldInteractableActor::CanInteract()
{
	 if (bIsPlaying)
	 	return false;

	for (const UInteractionCondition* Condition : StartConditions)
	{
		if (!Condition || !Condition->IsFulfilled()) return false;
	}
	
	return true;
}

void AWorldInteractableActor::Interact_Implementation(AActor* Interactor)
{	
	OnInteractionStarted.Broadcast(Interactor);
	
	/** 자신이 가진 Fragment 컴포넌트들 전부 가져와 TryExecuteInteraction 호출
	 *  상호작용에서 사용할 기능들은 IntractionFragment의 ExcuteInteraction에 구현
	 *  TryExecuteInteraction은 상호작용 실행 전 검사하는 함수 -> 사용 가능하면 알아서 ExecuteInteraction 호출
	 */
	
	TArray<UInteractionFragment*> Fragments;
	GetComponents<UInteractionFragment>(Fragments);

	for (UInteractionFragment* Fragment : Fragments)
	{
		if (Fragment)
		{
			Fragment->TryExecuteInteraction(Interactor);
		}
	}
}

void AWorldInteractableActor::EndInteract_Implementation()
{
	for (UInteractionCondition* Condition : ClearConditions)
	{
		if (Condition)
		{
			Condition->Reset();
		}
	}
	
	OnInteractionEnded.Broadcast(this);
	
	TArray<UInteractionFragment*> Fragments;
	GetComponents<UInteractionFragment>(Fragments);

	for (UInteractionFragment* Fragment : Fragments)
	{
		if (Fragment)
		{
			Fragment->EndInteraction();
		}
	}
}

bool AWorldInteractableActor::AreAllConditionsMet(TArray<TObjectPtr<UInteractionCondition>>& Conditions)
{
	for (auto Condition : Conditions)
	{
		if (!Condition->IsFulfilled()) return false;
	}

	return true;
}
