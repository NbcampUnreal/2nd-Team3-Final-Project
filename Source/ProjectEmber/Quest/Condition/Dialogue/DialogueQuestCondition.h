// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/Condition/QuestCondition.h"
#include "DialogueQuestCondition.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UDialogueQuestCondition : public UQuestCondition
{
	GENERATED_BODY()

public:

	virtual bool OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData) override;
	virtual FString GetConditionText_Implementation() const override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Condition")
	TArray<FText> DialogueLines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Condition")
	TSoftObjectPtr<AActor> TargetNPCActor;

};
