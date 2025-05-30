// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueQuestCondition.h"

bool UDialogueQuestCondition::OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
{
    if (InEventTag != EventTag)
    {
        return false;
    }
    if (EventData.Target && EventData.Target->IsA(TargetNPCClass))
    {
        CurrentCount++;
        return IsFulfilled();
    }
    return false;
}