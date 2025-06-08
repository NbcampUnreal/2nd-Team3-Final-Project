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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Condition")
	TSubclassOf<AActor> TargetNPCClass;
	virtual bool OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Condition")
	TArray<FText> DialogueLines;

};
