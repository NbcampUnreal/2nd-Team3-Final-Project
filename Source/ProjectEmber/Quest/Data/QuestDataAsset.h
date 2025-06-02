// QuestDataAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "../Condition/QuestCondition.h" 
#include "Abilities/GameplayAbilityTypes.h"
#include "QuestDataAsset.generated.h"


USTRUCT(BlueprintType)
struct FQuestRewardData
{
    GENERATED_BODY()

    // 보상 식별용 태그 ( 최대한 태그로 통일 )
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
    FGameplayTag RewardTag;

    // 보상 수량
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
    int32 RewardCount;
};

/**
 * 퀘스트의 개별 단계
 */
USTRUCT(BlueprintType)
struct FQuestStep
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    FName StepID;
    // 퀘스트 이름 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    FText StepName;

    // 퀘스트 설명 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    FText StepQuestDescription;

    //퀘스트 위치 설명 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    FText StepTargetLocationTag;

    //퀘스트 대상 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    FText ObjectiveName;

    // 이 단계에서 상호작용할 퀘스트 NPC (월드 인스턴스)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    TSoftObjectPtr<AActor> QuestGiver;

    // 대화 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    TArray<FText> GiverDialogueLines;
    
    // 진행 조건들
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Quest|Step")
    TArray<UQuestCondition*> Conditions;

    // 완료시 보고할 NPC
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    TSoftObjectPtr<AActor> CompletionGiver;

    // 완료시 출력할 대화 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    TArray<FText> CompleteDialogueLines;
    
    // 완료 시 수여할 보상 목록
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
    TArray<FQuestRewardData> Rewards;

    // 해금 조건들
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Quest|Step")
    TArray<UQuestCondition*> UnlockConditions;
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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Config")
    FName QuestID;

    // 퀘스트 이름 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Config")
    FText QuestName;

    // 퀘스트 설명 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Config")
    FText QuestDescription;

    //퀘스트 위치 설명 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Config")
    FText TargetLocationTag;

    // 퀘스트 단계를 순서대로 나열
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Config")
    TArray<FQuestStep> Steps;
};