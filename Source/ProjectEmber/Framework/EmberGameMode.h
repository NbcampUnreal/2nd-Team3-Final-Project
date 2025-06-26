// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EmberGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API AEmberGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	void OnAchievementsWritten(const FUniqueNetId& PlayerId, bool bSuccess);
};
