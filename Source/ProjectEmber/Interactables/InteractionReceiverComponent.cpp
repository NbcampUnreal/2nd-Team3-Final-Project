// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractionReceiverComponent.h"

#include "Condition/InteractionCondition.h"

UInteractionReceiverComponent::UInteractionReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionReceiverComponent::EvaluateDeactivationConditions(const TArray<UInteractionCondition*>& Conditions)
{
	bCanBeTriggeredAgain = true;

	if (Conditions.Num() == 0)
	{
		// 조건이 없으면 기본 true 유지
		return;
	}

	for (const UInteractionCondition* Condition : Conditions)
	{
		if (!Condition || !Condition->IsFulfilled())
		{
			// 하나라도 실패하면 다시 사용 가능 상태 유지
			bCanBeTriggeredAgain = true;
			return;
		}
	}

	// 모두 충족된 경우만 false
	bCanBeTriggeredAgain = false;
}


void UInteractionReceiverComponent::BroadCastInteractionCompleted(AActor* CompletedBy)
{
	OnInteractionCompleted.Broadcast(CompletedBy, bCanBeTriggeredAgain);
}

