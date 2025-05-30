#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueDataRow.generated.h"




USTRUCT(BlueprintType)
struct FDialogueDataRow : public FTableRowBase
{
    GENERATED_BODY()


    FDialogueDataRow()
        : RelatedQuestID(-1)
     
    {
        DialogueLines.Add(TEXT("안녕하세요, 여행자님."));
    }
    // 실제 대사
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> DialogueLines;

    // 퀘스트 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RelatedQuestID;



};