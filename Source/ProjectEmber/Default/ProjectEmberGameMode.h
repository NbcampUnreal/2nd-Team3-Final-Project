// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectEmberGameMode.generated.h"

UCLASS(minimalapi)
class AProjectEmberGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectEmberGameMode();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

private:
	UUserWidget* CurrentWidget;
};



