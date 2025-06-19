// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/WorldInteractable/WorldInteractableActor.h"
#include "MiniGameInteractableActor.generated.h"

class UMiniGameCondition;
struct FGameplayEventData;
class UQuestCondition;
class AMiniGameInteractableActor;

UCLASS()
class PROJECTEMBER_API AMiniGameInteractableActor : public AWorldInteractableActor
{
	GENERATED_BODY()
public:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void EndInteract_Implementation() override;
protected:
	void BeginMiniGame();
	void EndMiniGame();

	UFUNCTION(Category="Conditions")
	void CheckConditionCompletion(const FGameplayTag& EventTag, const FGameplayEventData& EventData);

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Conditions")
	TArray<TObjectPtr<UQuestCondition>> QuestConditions;
	
	bool bIsPlaying = false;
};

