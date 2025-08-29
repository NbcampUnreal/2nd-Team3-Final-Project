// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Fragments/InteractionFragment.h"
#include "InteractionFragment_PlayLS.generated.h"

class UStreamingSourceProvider;
class ALevelSequenceActor;
class ULevelSequencePlayer;
class ULevelSequence;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelSequenceFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStreamingStatusUpdated, bool, bIsCompleted);


UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInteractionFragment_PlayLS : public UInteractionFragment
{
	GENERATED_BODY()

public:
	UInteractionFragment_PlayLS();
	
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TObjectPtr<ALevelSequenceActor> LevelSequenceActor;

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnLevelSequenceFinished OnSequenceFinishedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Streaming")
	FOnStreamingStatusUpdated OnStreamingStatusUpdated;
private:
	UPROPERTY()
	TObjectPtr<UStreamingSourceProvider> StreamingSourceProvider;

	void WaitForStreamingAndPlaySequence(UWorldPartitionSubsystem* Subsystem);
	
	void StartSequencePlayback();
	
	UFUNCTION()
	void OnSequenceFinished();
};
