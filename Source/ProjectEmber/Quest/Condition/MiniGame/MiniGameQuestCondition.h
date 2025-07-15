// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/Condition/QuestCondition.h"
#include "Interactables/WorldInteractableActor.h"
#include "MiniGameQuestCondition.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UMiniGameQuestCondition : public UQuestCondition
{
	GENERATED_BODY()

public:
	virtual bool OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData) override;
	virtual FString GetConditionText_Implementation() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Condition")
	TSoftObjectPtr<AWorldInteractableActor> MinigameTarget;

};
