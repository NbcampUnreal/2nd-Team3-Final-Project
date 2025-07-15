// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Fragments/InteractionFragment_EventSender.h"

#include "GameInstance/GameplayEventSubsystem.h"
#include "Interactables/GameplayTagPayload.h"
#include "Interactables/Condition/InteractionCondition.h"
#include "MessageBus/MessageBus.h"

UInteractionFragment_EventSender::UInteractionFragment_EventSender()
{
}

void UInteractionFragment_EventSender::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);

	UMessageBus::GetInstance()->Subscribe("Interaction", MessageDelegate);
}

void UInteractionFragment_EventSender::EndInteraction_Implementation()
{
	Super::EndInteraction_Implementation();

	UMessageBus::GetInstance()->Unsubscribe("Interaction", MessageDelegate);

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

void UInteractionFragment_EventSender::OnMessageReceived(const FName& MessageType, UObject* Payload)
{
	if (UGameplayTagPayload* TagPayload = Cast<UGameplayTagPayload>(Payload))
	{
		if (TagPayload->GameplayTag.IsValid())
		{
			FGameplayEventData EventData;
			EventData.Instigator = TagPayload->ContextObject ? TagPayload->ContextObject : GetOwner();

			for (const FConditionalGameplayEvent& Entry : ConditionalEventsToSend)
			{
				if (IsValid(Entry.Condition) && Entry.Condition->OnEvent(TagPayload->GameplayTag, EventData))
				{
					if (Entry.Condition->IsFulfilled() && Entry.EventToSend.IsValid())
					{
						QueuedEvents.AddUnique(Entry.EventToSend);
					}
				}
			}
		}
	}
	
}
