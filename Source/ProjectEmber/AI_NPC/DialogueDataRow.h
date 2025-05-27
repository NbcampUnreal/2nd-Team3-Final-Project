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
        DialogueLines.Add(TEXT("�ȳ��ϼ���, �����ڴ�."));
    }
    // ���� ���
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> DialogueLines;

    // ����Ʈ ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RelatedQuestID;

    // ����Ʈ �� � ����(��ǥ)���� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ObjectiveTag;

    // ��� �ܰ迡�� ��µǴ� �������
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDialogueStage DialogueStage;
};