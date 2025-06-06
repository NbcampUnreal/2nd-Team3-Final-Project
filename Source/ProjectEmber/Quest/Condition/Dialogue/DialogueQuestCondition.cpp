#include "DialogueQuestCondition.h"
#include "AI_NPC/NPC_Component/DialogueComponent.h"

bool UDialogueQuestCondition::OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
{
    UE_LOG(LogTemp, Warning, TEXT("[DialogueCondition] OnEvent() called"));
    UE_LOG(LogTemp, Warning, TEXT("[Condition] InEventTag: %s / Expected Tag: %s"), *InEventTag.ToString(), *EventTag.ToString());

    if (InEventTag != EventTag)
        return false;

    if (!EventData.Target || !EventData.Target->IsA(TargetNPCClass))
        return false;

    CurrentCount++; // 카운트 증가

    bool bNowFulfilled = IsFulfilled();
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