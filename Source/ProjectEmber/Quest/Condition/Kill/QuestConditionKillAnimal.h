// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "Quest/Condition/QuestCondition.h"
#include "QuestConditionKillAnimal.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UQuestConditionKillAnimal : public UQuestCondition
{
	GENERATED_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Condition")
	TSoftClassPtr<AActor> TargetNPCClass;

	virtual FString GetConditionText_Implementation() const override;
	virtual bool OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData) override;
};
