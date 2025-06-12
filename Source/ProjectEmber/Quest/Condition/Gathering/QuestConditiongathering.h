// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/Condition/QuestCondition.h"
#include "QuestConditiongathering.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UQuestConditiongathering : public UQuestCondition
{
	GENERATED_BODY()

public:
    virtual bool OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData) override;

    virtual FString GetConditionText_Implementation() const override;

    //  기존 FName → FGameplayTag로 변경
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Condition")
    FGameplayTag TargetItemTag;
};