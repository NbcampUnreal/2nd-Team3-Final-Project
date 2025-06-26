// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/InteractionFragment_EventSender.h"

#include "Condition/InteractionCondition.h"
#include "GameInstance/GameplayEventSubsystem.h"

UInteractionFragment_EventSender::UInteractionFragment_EventSender()
{
}

void UInteractionFragment_EventSender::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);

	if (UGameplayEventSubsystem* Subsystem = UGameplayEventSubsystem::GetGameplayEvent(this))
	{
		Subsystem->OnGameEvent.AddDynamic(this, &UInteractionFragment_EventSender::OnGameplayEventReceived);
	}
}

void UInteractionFragment_EventSender::EndInteraction_Implementation()
{
	Super::EndInteraction_Implementation();

	if (UGameplayEventSubsystem* Subsystem = UGameplayEventSubsystem::GetGameplayEvent(this))
	{
		Subsystem->OnGameEvent.RemoveDynamic(this, &UInteractionFragment_EventSender::OnGameplayEventReceived);
	}

	for (const FGameplayTag& Tag : QueuedEvents)
	{
		TryBroadcastEvent(Tag);
	}

	QueuedEvents.Empty();
}

void UInteractionFragment_EventSender::TryBroadcastEvent(const FGameplayTag& EventTag)
{
	if (!EventTag.IsValid())
	{
		return;
	}
	AActor* Owner = GetOwner();
	if (UGameplayEventSubsystem* Subsystem = UGameplayEventSubsystem::GetGameplayEvent(Owner))
	{
		FGameplayEventData EventData;
		EventData.Instigator = Owner;
		Subsystem->BroadcastGameEvent(EventTag, EventData);
	}
}

void UInteractionFragment_EventSender::OnGameplayEventReceived(const FGameplayTag& EventTag,
	const FGameplayEventData& EventData)
{
	for (const FConditionalGameplayEvent& Entry : ConditionalEventsToSend)
	{
		if (Entry.Condition->OnEvent(EventTag, EventData))
		{
			if (Entry.Condition->IsFulfilled() && Entry.EventToSend.IsValid())
			{
				QueuedEvents.AddUnique(Entry.EventToSend);
			}
		}
	}
}
