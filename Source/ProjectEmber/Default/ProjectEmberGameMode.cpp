// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectEmberGameMode.h"
#include "ProjectEmberPlayerController.h"
#include "ProjectEmberCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AProjectEmberGameMode::AProjectEmberGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AProjectEmberPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownContent/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDownContent/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}*/
}

void AProjectEmberGameMode::BeginPlay()
{
    Super::BeginPlay();

    FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true); // true: streaming 없이 전체 이름

    if (CurrentLevelName.Equals("TestMap"))
    {
        if (MainMenuWidgetClass)
        {
            APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
            if (PC)
            {
                UUserWidget* Menu = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass);
                if (Menu)
                {
                    Menu->AddToViewport();
                    PC->bShowMouseCursor = true;
                    PC->SetInputMode(FInputModeUIOnly());
                }
            }
        }
    }
}