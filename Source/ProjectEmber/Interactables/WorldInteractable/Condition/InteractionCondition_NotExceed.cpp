// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/Condition/InteractionCondition_NotExceed.h"


bool UInteractionCondition_NotExceed::IsFulfilled() const
{
	return CurrentCount < RequiredCount;
}
