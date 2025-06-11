// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/WorldInteractable/WorldInteractableActor.h"
#include "MiniGameInteractableActor.generated.h"

class AMiniGameInteractableActor;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class PROJECTEMBER_API UMiniGameFragment : public UObject
{
	GENERATED_BODY()
public:
	virtual void OnMiniGameStart(AMiniGameInteractableActor* Owner) {}
	virtual void OnMiniGameEnd() {}
};

UCLASS()
class PROJECTEMBER_API AMiniGameInteractableActor : public AWorldInteractableActor
{
	GENERATED_BODY()
public:
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	void BeginMiniGame();
	void EndMiniGame();
	
	virtual bool IsMiniGameCleared() const { return false; }
	virtual void GiveReward(AActor* Receiver) {}

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="MiniGame")
	TArray<UMiniGameFragment*> MiniGameFragments;
};

