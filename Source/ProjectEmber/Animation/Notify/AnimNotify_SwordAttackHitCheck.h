// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTags.h"
#include "AnimNotify_SwordAttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UAnimNotify_SwordAttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()
public:
	UAnimNotify_SwordAttackHitCheck();
	
protected:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = Notify)
	FGameplayTag TriggerGameplayTag;

	UPROPERTY(EditAnywhere, Category = Notify)
	float ComboAttackLevel;
};
