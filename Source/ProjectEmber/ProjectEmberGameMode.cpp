// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectEmberGameMode.h"
#include "ProjectEmberPlayerController.h"
#include "ProjectEmberCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectEmberGameMode::AProjectEmberGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AProjectEmberPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownContent/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDownContent/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}