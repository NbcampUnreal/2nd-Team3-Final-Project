// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CineCameraComponent.h"
#include "AttatchAIDialogueCamera.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTEMBER_API UAttatchAIDialogueCamera : public UCineCameraComponent
{
	GENERATED_BODY()
	

public:
	UAttatchAIDialogueCamera();

protected:
	virtual void BeginPlay() override;
};