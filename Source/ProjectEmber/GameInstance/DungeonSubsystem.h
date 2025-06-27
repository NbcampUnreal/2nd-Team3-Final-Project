// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DungeonSubsystem.generated.h"

class ADungeonGenerator;
class ADoor;

UCLASS()
class PROJECTEMBER_API UDungeonSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetTempDungeonGenerator(ADungeonGenerator* DungeonGenerator);

	UFUNCTION(BlueprintCallable)
	ADungeonGenerator* GetTempDungeonGenerator();

	UFUNCTION(BlueprintCallable)
	void SetTempBossDoor(ADoor* BossDoor);

	UFUNCTION(BlueprintCallable)
	ADoor* GetTempBossDoor();

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "DungeonGenerator")
	TObjectPtr<ADungeonGenerator> TempDungeonGenerator;

	UPROPERTY(EditDefaultsOnly, Category = "DungeonBossDoor")
	TObjectPtr<ADoor> TempBossDoor;
};
