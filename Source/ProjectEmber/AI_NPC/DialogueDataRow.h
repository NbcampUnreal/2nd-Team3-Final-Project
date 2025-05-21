#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueDataRow.generated.h"

USTRUCT(BlueprintType)
struct FDialogueDataRow : public FTableRowBase
{
    GENERATED_BODY()

    FDialogueDataRow()
    {
        DialogueLines.Add(TEXT("æ»≥Á«œººø‰, ø©«‡¿⁄¥‘."));
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> DialogueLines;
};