#include "QuestConditiongathering.h"

bool UQuestConditiongathering::OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
{
    if (InEventTag == EventTag && EventData.TargetTags.HasTagExact(TargetItemTag))
    {
        CurrentCount++;
        return IsFulfilled();
    }
    return false;
}

FString UQuestConditiongathering::GetConditionText_Implementation() const
{
    FString NameStr = ConditionName.IsEmpty() ? TargetItemTag.ToString() : ConditionName.ToString();

    return FString::Printf(TEXT("%s: %d / %d"), *NameStr, CurrentCount, RequiredCount);
}