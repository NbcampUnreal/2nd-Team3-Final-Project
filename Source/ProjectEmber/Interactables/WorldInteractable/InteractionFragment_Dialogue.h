// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/WorldInteractable/InteractionFragment.h"
#include "InteractionFragment_Dialogue.generated.h"

class UDialogueComponent;
class UEmberInputHandlerComponent;
/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInteractionFragment_Dialogue : public UInteractionFragment
{
	GENERATED_BODY()
public:
	virtual bool CanExecuteInteraction_Implementation(AActor* Interactor) const override;

	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;

	virtual void EndInteraction_Implementation() override;

private:
	TObjectPtr<UDialogueComponent> DialogueComponent;
	TObjectPtr<UEmberInputHandlerComponent> InputHandlerComponent;
};
