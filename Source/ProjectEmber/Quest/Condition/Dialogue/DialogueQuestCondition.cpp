#include "DialogueQuestCondition.h"
#include "AI_NPC/NPC_Component/DialogueComponent.h"

bool UDialogueQuestCondition::OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
{
    UE_LOG(LogTemp, Warning, TEXT("[DialogueCondition] OnEvent() called"));
    UE_LOG(LogTemp, Warning, TEXT("[Condition] InEventTag: %s / Expected Tag: %s"), *InEventTag.ToString(), *EventTag.ToString());

    if (InEventTag != EventTag)
        return false;

    if (!EventData.Target)
        return false;

    //  오직 특정 Actor만 조건 만족
    if (!TargetNPCActor.IsValid() || EventData.Target != TargetNPCActor.Get())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueCondition] Event 대상이 지정한 NPC와 다릅니다."));
        return false;
    }

    CurrentCount++;

    const bool bNowFulfilled = IsFulfilled();
    if (bNowFulfilled)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueCondition] 조건 충족 완료! Target: %s / 현재 카운트: %d"),
            *EventData.Target->GetName(), CurrentCount);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueCondition] 조건 아직 미충족. Target: %s / 현재 카운트: %d / 필요 수: %d"),
            *EventData.Target->GetName(), CurrentCount, RequiredCount);
    }

    return bNowFulfilled;
}
FString UDialogueQuestCondition::GetConditionText_Implementation() const
{
    FString NameStr = ConditionName.ToString();

    return FString::Printf(TEXT("%s: %d / %d"), *NameStr, CurrentCount, RequiredCount);
}