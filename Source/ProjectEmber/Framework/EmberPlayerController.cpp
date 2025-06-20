#include "EmberPlayerController.h"

#include "EMSFunctionLibrary.h"
#include "EMSInfoSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "Components/SceneCaptureComponent2D.h"
#include "EmberLog/EmberLog.h"
#include "FunctionLibrary/UIFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AEmberPlayerController::AEmberPlayerController()
{
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture2D"));

	SceneCaptureComponent2D->SetupAttachment(GetRootComponent());
	SceneCaptureComponent2D->bCaptureEveryFrame = false;
	SceneCaptureComponent2D->bCaptureOnMovement = false;
}

void AEmberPlayerController::ActorPreSave_Implementation()
{
	SaveThumbnailImage();
}

void AEmberPlayerController::ActorLoaded_Implementation()
{
	IEMSActorSaveInterface::ActorLoaded_Implementation();
}

void AEmberPlayerController::SaveThumbnailImage()
{
	if (!ThumbnailRenderTarget || !SceneCaptureComponent2D)
	{
		EMBER_LOG(LogEmber, Warning, TEXT("SaveThumbnailImage: RenderTarget or SceneCapture missing"));
		return;
	}
	
	SceneCaptureComponent2D->TextureTarget = ThumbnailRenderTarget;
	SceneCaptureComponent2D->CaptureScene();
	
	FString SlotName;
	if (UEMSInfoSaveGame* Info = UEMSFunctionLibrary::GetSlotInfoSaveGame(this, SlotName))
	{
		UEMSFunctionLibrary::ExportSaveThumbnail(this, SceneCaptureComponent2D->TextureTarget, SlotName);
		EMBER_LOG(LogEmber, Warning, TEXT("Thumbnail exported for slot: %s"), *SlotName);
	}
	else
	{
		EMBER_LOG(LogEmber, Warning, TEXT("SaveThumbnailImage: Failed to get current save slot")); 
	}
}

void AEmberPlayerController::SaveGameToSlot()
{
	CloseSaveMenuWidget();

	SaveThumbnailImage();

	SaveCustom();

	UEMSAsyncSaveGame* Active = UEMSAsyncSaveGame::AsyncSaveActors(GetWorld(), 1);
	Active->OnCompleted.AddDynamic(this, &AEmberPlayerController::ShowSavingMessage);
}

void AEmberPlayerController::LoadGameToSlot()
{
	CloseSaveMenuWidget();
	
	FString SlotName;
	if (const UEMSInfoSaveGame* SlotInfo = UEMSFunctionLibrary::GetSlotInfoSaveGame(this, SlotName))
	{
		UGameplayStatics::OpenLevel(GetWorld(), SlotInfo->SlotInfo.Level, true);	
	}
}

void AEmberPlayerController::OpenSaveMenu()
{
	OpenSaveMenuWidget();
}

void AEmberPlayerController::CloseSaveMenuWidget()
{
	if (IsValid(SaveGameMenuWidget))
	{
		SaveGameMenuWidget->RemoveFromParent();
		UUIFunctionLibrary::FocusGame(this);

		SaveGameMenuWidget = nullptr;
	}
}

void AEmberPlayerController::SaveCustom()
{
	
}

void AEmberPlayerController::ShowSavingMessage()
{
	if (IsValid(SaveGameMessageWidget))
	{
		SaveGameMessageWidget->RemoveFromParent();
	}
	
	SaveGameMessageWidget = CreateWidget(this, SaveGameMessageWidgetClass);
	if (SaveGameMessageWidget)
	{
		SaveGameMessageWidget->AddToViewport();
	}
}
