// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestConditiongathering.h"

bool UQuestConditiongathering::OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
{
   
    if (InEventTag == EventTag)   
    {
        CurrentCount++;         
        return IsFulfilled();   
    }
    return false;
}