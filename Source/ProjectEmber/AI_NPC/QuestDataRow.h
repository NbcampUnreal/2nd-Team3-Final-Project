// QuestDataRow.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestDataRow.generated.h"

USTRUCT(BlueprintType)
struct FQuestDataRow : public FTableRowBase
{
    GENERATED_BODY()

    FQuestDataRow()
        : QuestID(0)
        , QuestName(TEXT("Unnamed Quest"))
        , Description(TEXT("No description"))
        , TargetLocation(TEXT("Location"))
        , ObjectiveNames()
        , ObjectiveGoals()
        , RewardGold(0)
        , RewardExp(0)
        , RelatedQuestID(NAME_None)
        , bIsQuestCompletionDialogue(false)
        , QuestType(TEXT("Main"))          
        , bHiddenInLog(false)               
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString QuestName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TargetLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> ObjectiveNames;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> ObjectiveGoals;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RewardGold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RewardExp;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RelatedQuestID;  

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsQuestCompletionDialogue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString QuestType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHiddenInLog = false;
};
