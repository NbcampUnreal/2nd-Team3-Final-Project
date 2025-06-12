// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/InteractionTriggerComponent.h"

#include "InteractionReceiverComponent.h"
#include "EmberLog/EmberLog.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UInteractionTriggerComponent::UInteractionTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInteractionTriggerComponent::ActivateInteractions()
{
	for (int i = 0; i < InteractionTargets.Num(); i++)
	{
		if (InteractionTargets[i].InteractionTarget)
		{
			UInteractionReceiverComponent* ReceiverComponent = InteractionTargets[i].InteractionTarget->GetComponentByClass<UInteractionReceiverComponent>();
			if (!ReceiverComponent) continue;

			AActor* CausingActor = GetOwner();

			switch (InteractionTargets[i].InteractionAction)
			{
			case EInteractionAction::Type::Action1:
				ReceiverComponent->ActivateAction1.Broadcast(CausingActor);
				break;
			case EInteractionAction::Type::Action2:
				ReceiverComponent->ActivateAction2.Broadcast(CausingActor);
				break;
			case EInteractionAction::Type::Action3:
				ReceiverComponent->ActivateAction3.Broadcast(CausingActor);
				break;
			case EInteractionAction::Type::Action4:
				ReceiverComponent->ActivateAction4.Broadcast(CausingActor);
				break;
			case EInteractionAction::Type::Action5:
				ReceiverComponent->ActivateAction5.Broadcast(CausingActor);
				break;
			case EInteractionAction::Type::Action6:
				ReceiverComponent->ActivateAction6.Broadcast(CausingActor);
				break;
			case EInteractionAction::Type::Action7:
				ReceiverComponent->ActivateAction7.Broadcast(CausingActor);
				break;
			case EInteractionAction::Type::Action8:
				ReceiverComponent->ActivateAction8.Broadcast(CausingActor);
				break;
			case EInteractionAction::Type::Action9:
				ReceiverComponent->ActivateAction9.Broadcast(CausingActor);
				break;
			case EInteractionAction::Type::Action10:
				ReceiverComponent->ActivateAction10.Broadcast(CausingActor);
				break;
			}
		}
	}
}

void UInteractionTriggerComponent::AddInteractionTarget(FName InstanceName,
                                                        TEnumAsByte<EInteractionAction::Type> InteractionAction)
{
	if (!GetWorld()) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(),InstanceName, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (Actor)
		{
			InteractionTargets.Add(FInteractionTarget(Actor, InteractionAction));
			UE_LOG(LogEmberInteraction, Warning, TEXT("Interaction Target Added : %s with Action %d"), *Actor->GetName(), (int32)InteractionAction);
		}
	}

	if (FoundActors.Num() == 0)
	{
		UE_LOG(LogEmberInteraction, Warning, TEXT("Can't find %s"), *InstanceName.ToString());
	}
}

