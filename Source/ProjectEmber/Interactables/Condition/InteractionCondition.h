// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/Condition/QuestCondition.h"
#include "InteractionCondition.generated.h"

UENUM(BlueprintType)
enum class EConditionEffectType : uint8
{
	Increase UMETA(DisplayName = "Increase"),
	Decrease UMETA(DisplayName = "Decrease"),
	Reset  UMETA(DisplayName = "Reset"),
	Ignore UMETA(DisplayName = "Ignore")
};

UCLASS()
class PROJECTEMBER_API UInteractionCondition : public UQuestCondition
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Condition")
	EConditionEffectType EvaluateEventEffect(const FGameplayTag& InEventTag, const FGameplayEventData& EventData) const;
	
	virtual bool OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData) override;

	
};
