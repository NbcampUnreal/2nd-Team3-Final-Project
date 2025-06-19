// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/WorldInteractable/InteractionFragment.h"
#include "InteractionFragment_PlayLS.generated.h"

class ALevelSequenceActor;
class ULevelSequencePlayer;
class ULevelSequence;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelSequenceFinished);

UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInteractionFragment_PlayLS : public UInteractionFragment
{
	GENERATED_BODY()

public:
	UInteractionFragment_PlayLS();
	
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TObjectPtr<ULevelSequence> LevelSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TObjectPtr<AActor> SequenceTargetActor = nullptr;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnLevelSequenceFinished OnSequenceFinishedEvent;
private:
	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> SequencePlayer;

	UPROPERTY(Transient)
	TObjectPtr<ALevelSequenceActor> SequenceActor;

	UFUNCTION()
	void OnSequenceFinished();
};
