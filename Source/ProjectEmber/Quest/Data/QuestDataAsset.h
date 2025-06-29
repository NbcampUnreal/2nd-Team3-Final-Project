// QuestDataAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "../Condition/QuestCondition.h" 
#include "Abilities/GameplayAbilityTypes.h"
#include "Tutorial/Subsystem/TutorialManagerSubsystem.h"
#include "QuestDataAsset.generated.h"



USTRUCT(BlueprintType)
struct FDialogueLines
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FText> Lines;
};

USTRUCT(BlueprintType)
struct FQuestRewardData
{
    GENERATED_BODY()

    // 보상 식별용 태그 ( 최대한 태그로 통일 )
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    FGameplayTag RewardTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    FName ItemID;
    
    // 보상 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    int32 RewardCount{1};
};

/**
 * 퀘스트의 개별 단계
 */
USTRUCT(BlueprintType)
struct FQuestStep
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    FName StepID;
    // 퀘스트 이름 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    FText StepName;

    // 퀘스트 박스 리스트 이름 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    FText StepQuestListName;

    // 퀘스트 설명 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    FText StepQuestDescription;

    //퀘스트 위치 설명 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    FText StepTargetLocationTag;

    //퀘스트 대상 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    FText StepObjectiveName;

    //퀘스트 종류 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    FText StepQuesttype;

    // 이 단계에서 상호작용할 퀘스트 NPC (월드 인스턴스)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    TSoftObjectPtr<AActor> QuestGiver;

    // 대화 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    TArray<FText> GiverDialogueLines;
    
    // 진행 조건들
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Quest|Step")
    TArray<UQuestCondition*> Conditions;

    // 완료시 보고할 NPC
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    TSoftObjectPtr<AActor> CompletionGiver;

    // 완료시 출력할 대화 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    TArray<FText> CompleteDialogueLines;
    
    // 완료 시 수여할 보상 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Step")
    TArray<FQuestRewardData> Rewards;

    // 해금 조건들
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Quest|Step")
    TArray<UQuestCondition*> UnlockConditions;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tutorial")
    bool bShowTutorial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tutorial")
    UTutorialDataAsset* TutorialDataAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> TutorialIndexes; 

};

/**
 * 전체 퀘스트 DataAsset
 */
UCLASS(BlueprintType)
class PROJECTEMBER_API UQuestDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    // 고유 퀘스트 식별자
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Config")
    FName QuestID;

    // 퀘스트 이름 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Config")
    FText QuestName;

    // 퀘스트 설명 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Config")
    FText QuestDescription;

    //퀘스트 위치 설명 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Config")
    FText TargetLocationTag;

    // 퀘스트 단계를 순서대로 나열
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Config")
    TArray<FQuestStep> Steps;
    
};
