// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionTypes.generated.h"

UENUM(BlueprintType)
namespace EInteractionAction
{
	enum Type
	{
		Action1 UMETA(DisplayName="Action1"),
		Action2 UMETA(DisplayName="Action2"),
		Action3 UMETA(DisplayName="Action3"),
		Action4 UMETA(DisplayName="Action4"),
		Action5 UMETA(DisplayName="Action5"),
		Action6 UMETA(DisplayName="Action6"),
		Action7 UMETA(DisplayName="Action7"),
		Action8 UMETA(DisplayName="Action8"),
		Action9 UMETA(DisplayName="Action9"),
		Action10 UMETA(DisplayName="Action10"),
	};
}

USTRUCT(BlueprintType)
struct PROJECTEMBER_API FInteractionTarget
{
	GENERATED_BODY()
	
	FInteractionTarget(AActor* InInteractionTarget = nullptr, EInteractionAction::Type InInteractionAction = EInteractionAction::Type::Action1)
		: InteractionTarget(InInteractionTarget), InteractionAction(InInteractionAction)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* InteractionTarget = nullptr;

	UPROPERTY(EditAnywhere, Category="FInteractionTarget")
	TEnumAsByte<EInteractionAction::Type> InteractionAction = EInteractionAction::Type::Action1;
};
