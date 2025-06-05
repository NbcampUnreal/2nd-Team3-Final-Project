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

    // (1) 아직 조건이 충족되지 않았다면(즉, 마지막 카운트에만 대사 출력)
    bool bWasFulfilled = IsFulfilled();

    CurrentCount++; // 카운트 증가

    bool bNowFulfilled = IsFulfilled();

    // (2) 조건 달성 순간에만 대사 세팅
    if (!bWasFulfilled && bNowFulfilled)
    {
        if (UDialogueComponent* DialogueComp = EventData.Target->FindComponentByClass<UDialogueComponent>())
        {
            DialogueComp->SetCustomDialogueLines(DialogueLines);
        }
    }

    return bNowFulfilled;
}