// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "DialogueCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API ADialogueCameraActor : public ACineCameraActor
{
	GENERATED_BODY()
	
public:
	ADialogueCameraActor(const FObjectInitializer& ObjectInitializer);
};
