#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueDataRow.generated.h"


UENUM(BlueprintType)
enum class EDialogueStage : uint8
{
    BeforeAccept     UMETA(DisplayName = "BeforeAccept"),
    AfterAccept      UMETA(DisplayName = "AfterAccept"),
    InProgress       UMETA(DisplayName = "InProgress"),
    BeforeComplete   UMETA(DisplayName = "BeforeComplete"),
    AfterComplete    UMETA(DisplayName = "AfterComplete")
};

USTRUCT(BlueprintType)
struct FDialogueDataRow : public FTableRowBase
{
    GENERATED_BODY()


    FDialogueDataRow()
        : RelatedQuestID(-1)
        , DialogueStage(EDialogueStage::BeforeAccept)
    {
        DialogueLines.Add(TEXT("안녕하세요, 여행자님."));
    }
    // 실제 대사
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> DialogueLines;

    // 퀘스트 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RelatedQuestID;

    // 퀘스트 내 어떤 조건(목표)인지 구분
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ObjectiveTag;

    // 어느 단계에서 출력되는 대사인지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDialogueStage DialogueStage;
};