// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlayGameplayEvent.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Play Gameplay Event"))
class EMBERABILITYSYSTEM_API UAnimNotify_PlayGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayEvent")
	FGameplayTag EventTag;
};
