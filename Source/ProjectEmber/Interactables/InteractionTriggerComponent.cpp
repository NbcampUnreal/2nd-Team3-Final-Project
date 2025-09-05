// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractionTriggerComponent.h"

#include "WorldInteractableActor.h"
#include "EmberLog/EmberLog.h"

UInteractionTriggerComponent::UInteractionTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionTriggerComponent::ActivateInteractions()
{
	for (int i = 0; i < InteractionTargets.Num(); i++)
	{
		if (InteractionTargets[i])
		{
			AActor* CausingActor = GetOwner();

			IInteractable::Execute_TryInteract(InteractionTargets[i], CausingActor);
		}
	}
}

void UInteractionTriggerComponent::AddInteractionTarget(FName InstanceName,
                                                        TEnumAsByte<EInteractionAction::Type> InteractionAction)
{
	//if (!GetWorld()) return;

	//TArray<AActor*> FoundActors;
	//UGameplayStatics::GetAllActorsWithTag(GetWorld(),InstanceName, FoundActors);
//
	//for (AActor* Actor : FoundActors)
	//{
	//	if (Actor)
	//	{
	//		InteractionTargets.Add(FInteractionTarget(Actor, InteractionAction));
	//		UE_LOG(LogEmberInteraction, Warning, TEXT("Interaction Target Added : %s with Action %d"), *Actor->GetName(), (int32)InteractionAction);
	//	}
	//}
//
	//if (FoundActors.Num() == 0)
	//{
	//	UE_LOG(LogEmberInteraction, Warning, TEXT("Can't find %s"), *InstanceName.ToString());
	//}
}

void UInteractionTriggerComponent::OnTargetInteractionCompleted(AActor* CompletedBy, bool bCanBeTriggeredAgain)
{
	//if (!bCanBeTriggeredAgain)
	//{
	//	// 해당 액터는 더 이상 활성화 대상 아님
	//	for (int32 i = 0; i < InteractionTargets.Num(); ++i)
	//	{
	//		if (InteractionTargets[i].InteractionTarget == CompletedBy)
	//		{
	//			InteractionTargets.RemoveAt(i);
	//			break;
	//		}
	//	}
//
	//	if (InteractionTargets.Num() == 0)
	//	{
	//		OnAllReceiversDeactivated.Broadcast();
	//	}
	//}
}
