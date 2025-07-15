// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/WorldInteractableActor.h"
#include "Loop/LoopResettable.h"
#include "LoopResettableInteractableActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API ALoopResettableInteractableActor : public AWorldInteractableActor, public ILoopResettable
{
	GENERATED_BODY()

public:
	virtual void EndInteract_Implementation() override;
	
	virtual void BeginPlay() override;
	///// 루프
	virtual void OnLoopReset_Implementation() override;

	virtual FGuid GetID_Implementation() const override;

	UFUNCTION(CallInEditor, Category="LoopResettable")
	void GenerateNewGuid();
	
	// 루프 매니저 식별자
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LoopResettable")
	FGuid PersistentActorID;
	
//#if WITH_EDITOR
//	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif
};
