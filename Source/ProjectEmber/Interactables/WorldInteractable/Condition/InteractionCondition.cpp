// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/Condition/InteractionCondition.h"

EConditionEffectType UInteractionCondition::EvaluateEventEffect_Implementation(const FGameplayTag& InEventTag,
	const FGameplayEventData& EventData) const
{
	return (InEventTag == EventTag) ? EConditionEffectType::Increase : EConditionEffectType::Ignore;
}

bool UInteractionCondition::OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
{
	const EConditionEffectType Effect = EvaluateEventEffect(InEventTag, EventData);
	
	switch (Effect)
	{
	case EConditionEffectType::Increase:
		CurrentCount++;
		break;
	case EConditionEffectType::Decrease:
		CurrentCount = FMath::Max(0, CurrentCount - 1);
		break;
	case EConditionEffectType::Reset:
		CurrentCount = 0;
		break;
	case EConditionEffectType::Ignore:
	default:
		return false;
	}

	return IsFulfilled();
}
