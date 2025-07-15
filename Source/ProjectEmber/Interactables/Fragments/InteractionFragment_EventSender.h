// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interactables/Fragments/InteractionFragment.h"
#include "MessageBus/MessageBus.h"
#include "InteractionFragment_EventSender.generated.h"

class UInteractionCondition;
struct EventData;
struct FGameplayEventData;

USTRUCT(BlueprintType)
struct FConditionalGameplayEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UInteractionCondition> Condition;

	UPROPERTY(EditAnywhere)
	FGameplayTag EventToSend;
};


UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInteractionFragment_EventSender : public UInteractionFragment
{
	GENERATED_BODY()

public:
	UInteractionFragment_EventSender();

	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;

	virtual void EndInteraction_Implementation() override;

	void TryBroadcastEvent(const FGameplayTag& EventTag);

	UFUNCTION()
	void OnMessageReceived(const FName& MessageType, UObject* Payload);
	
	UPROPERTY(EditAnywhere, Category="Interaction")
	bool bSendOnlyIfConditionMet = true;

	UPROPERTY(EditAnywhere, Category="Interaction")
	TArray<FConditionalGameplayEvent> ConditionalEventsToSend;

	TArray<FGameplayTag> QueuedEvents;

	FMessageDelegate MessageDelegate;
};

