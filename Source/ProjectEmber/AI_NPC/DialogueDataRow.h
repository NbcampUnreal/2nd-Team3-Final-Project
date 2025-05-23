#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueDataRow.generated.h"

USTRUCT(BlueprintType)
struct FDialogueDataRow : public FTableRowBase
{
    GENERATED_BODY()

    FDialogueDataRow()
        : bStartQuest(false)
        , bIsQuestCompletionDialogue(false)
        , RelatedQuestID(-1)
    {
        DialogueLines.Add(TEXT("æ»≥Á«œººø‰, ø©«‡¿⁄¥‘."));
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> DialogueLines;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bStartQuest;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bIsQuestCompletionDialogue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RelatedQuestID;
};