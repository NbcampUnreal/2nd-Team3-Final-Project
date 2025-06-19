// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/WorldInteractableActor.h"

#include "Condition/InteractionCondition.h"
#include "InteractionFragment.h"
#include "InteractionReceiverComponent.h"


AWorldInteractableActor::AWorldInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	ReceiverComponent = CreateDefaultSubobject<UInteractionReceiverComponent>(TEXT("ReceiverComponent"));
	ReceiverComponent->ActivateAction1.AddDynamic(this, &AWorldInteractableActor::TryInteract);
}

void AWorldInteractableActor::HandleInteractionEvent_Implementation(const FGameplayTag& EventTag,
	const FGameplayEventData& EventData)
{
	for (UInteractionCondition* Condition : InteractConditions)
	{
		if (Condition)
		{
			Condition->OnEvent(EventTag, EventData);
		}
	}

	for (UInteractionCondition* Condition : DeactivateConditions)
	{
		if (Condition)
		{
			Condition->OnEvent(EventTag, EventData);
		}
	}

	if (ReceiverComponent)
	{
		ReceiverComponent->EvaluateDeactivationConditions(DeactivateConditions);
	}
	
	if (!ReceiverComponent->bCanBeTriggeredAgain)
	{
		if (this->Implements<UInteractable>())
		{
			IInteractable::Execute_EndInteract(this);
		}
	}
}

void AWorldInteractableActor::TryInteract(AActor* Interactor)
{
	if (CanInteract())
	{
		IInteractable::Execute_Interact(this, Interactor);
	}
}

bool AWorldInteractableActor::CanInteract()
{
	for (const UInteractionCondition* Condition : InteractConditions)
	{
		if (!Condition || !Condition->IsFulfilled()) return false;
	}
	
	return true;
}

bool AWorldInteractableActor::IsDeactivate()
{
	for (const UInteractionCondition* Condition : DeactivateConditions)
	{
		if (!Condition || !Condition->IsFulfilled()) return false;
	}
	
	return true;
}

void AWorldInteractableActor::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);
	
	OnInteractionStarted.Broadcast(Interactor);

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
	TArray<UInteractionFragment*> Fragments;
	GetComponents<UInteractionFragment>(Fragments);

	for (UInteractionFragment* Fragment : Fragments)
	{
		if (Fragment)
		{
			Fragment->EndInteraction();
		}
	}

	for (UInteractionCondition* Condition : InteractConditions)
	{
		if (Condition)
		{
			Condition->Reset();
		}
	}
	for (UInteractionCondition* Condition : DeactivateConditions)
	{
		if (Condition)
		{
			Condition->Reset();
		}
	}
	
	if (ReceiverComponent)
	{
		ReceiverComponent->BroadCastInteractionCompleted(this); // 트리거에게 알리기
	}

	OnInteractionEnded.Broadcast(this, ReceiverComponent->bCanBeTriggeredAgain); // 미니게임이 true 보낸다면 클리어니 상자에 open 태그 보내기
}

