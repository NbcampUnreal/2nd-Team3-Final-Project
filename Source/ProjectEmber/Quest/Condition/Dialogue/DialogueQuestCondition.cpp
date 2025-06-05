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

    // (1) ���� ������ �������� �ʾҴٸ�(��, ������ ī��Ʈ���� ��� ���)
    bool bWasFulfilled = IsFulfilled();

    CurrentCount++; // ī��Ʈ ����

    bool bNowFulfilled = IsFulfilled();

    // (2) ���� �޼� �������� ��� ����
    if (!bWasFulfilled && bNowFulfilled)
    {
        if (UDialogueComponent* DialogueComp = EventData.Target->FindComponentByClass<UDialogueComponent>())
        {
            DialogueComp->SetCustomDialogueLines(DialogueLines);
        }
    }

    return bNowFulfilled;
}