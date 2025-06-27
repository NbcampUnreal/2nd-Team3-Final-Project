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

void AWorldInteractableActor::TryInteract_Implementation(AActor* Interactor)
{
	//ReceiverComponent->GetTri
	
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
	
	/** 자신이 가진 Fragment 컴포넌트들 전부 가져와 TryExecuteInteraction 호출
	 *  상호작용에서 사용할 기능들은 IntractionFragment의 ExcuteInteraction에 구현
	 *  TryExecuteInteraction은 상호작용 실행 전 검사하는 함수 -> 사용 가능하면 알아서 ExecuteInteraction 호출
	 */

	FGameplayTag ResultTag = FGameplayTag::RequestGameplayTag("Interaction.Started");

	if (ResultTag.IsValid())
	{
		FGameplayEventData EndData;
		EndData.Instigator = this;
		UGameplayEventSubsystem::GetGameplayEvent(this)->BroadcastGameEvent(ResultTag, EndData);
	}
	
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
	// 결과 브로드캐스팅 -> 지금은 퀘스트에서만 받을 예정
	FGameplayEventData EventData;
	EventData.Instigator = GetOwner();
	
	FGameplayTag ResultTag = FGameplayTag::RequestGameplayTag("Interaction.Reusable");
	if (!ReceiverComponent->bCanBeTriggeredAgain)
	{
		ResultTag = FGameplayTag::RequestGameplayTag("Interaction.Cleared");
	}
	if (ResultTag.IsValid())
	{
		FGameplayEventData EndData;
		EndData.Instigator = this;
		UGameplayEventSubsystem::GetGameplayEvent(this)->BroadcastGameEvent(ResultTag, EndData);
	}
	
	if (ReceiverComponent)
	{
		ReceiverComponent->BroadCastInteractionCompleted(this); // 트리거에게 알리기
	}

	OnInteractionEnded.Broadcast(this, ReceiverComponent->bCanBeTriggeredAgain); // 미니게임이 true 보낸다면 클리어니 상자에 open 태그 보내기
	
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