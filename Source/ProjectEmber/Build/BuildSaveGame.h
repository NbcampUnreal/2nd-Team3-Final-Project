// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Build/BuildSaveData.h"
#include "EMSCustomSaveGame.h"
#include "BuildSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBuildSaveGame : public UEMSCustomSaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(SaveGame)
	TArray<FBuildSaveData> SavedBuilds; // FBuildSaveData�� ��ġ/ȸ��/������ �� �ǹ� ������ ����� Struct
};
