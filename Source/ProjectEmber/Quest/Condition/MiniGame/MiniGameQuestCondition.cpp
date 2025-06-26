#include "MiniGameQuestCondition.h"


bool UMiniGameQuestCondition::OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
{
    if (InEventTag != EventTag)
        return false;

    if (!EventData.Instigator)
        return false;
    
    if (!MinigameTarget.IsValid() || EventData.Instigator != MinigameTarget.Get())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueCondition] Event ����� ������ NPC�� �ٸ��ϴ�."));
        return false;
    }

    CurrentCount++;

    const bool bNowFulfilled = IsFulfilled();
    if (bNowFulfilled)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueCondition] ���� ���� �Ϸ�! Target: %s / ���� ī��Ʈ: %d"),
            *EventData.Instigator->GetName(), CurrentCount);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueCondition] ���� ���� ������. Target: %s / ���� ī��Ʈ: %d / �ʿ� ��: %d"),
            *EventData.Instigator->GetName(), CurrentCount, RequiredCount);
    }

    return bNowFulfilled;
}
FString UMiniGameQuestCondition::GetConditionText_Implementation() const
{
    FString NameStr = ConditionName.ToString();

    return FString::Printf(TEXT("%s: %d / %d"), *NameStr, CurrentCount, RequiredCount);
}