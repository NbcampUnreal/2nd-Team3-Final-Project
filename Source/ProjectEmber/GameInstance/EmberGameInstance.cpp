#include "GameInstance/EmberGameInstance.h"
#include "GameInstance/AudioSubsystem.h"
#include "GameInstance/LevelSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void UEmberGameInstance::Init()
{
	Super::Init();

	AudioSubsystem = GetSubsystem<UAudioSubsystem>();
	LevelSubsystem = GetSubsystem<ULevelSubsystem>();
}

void UEmberGameInstance::TestPlaySound()
{
	AudioSubsystem->PlayBGMSoundByArea(EAreaType::LobbyArea);
}

void UEmberGameInstance::ShowLoadingScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("[Loading] ShowLoadingScreen called"));

	if (!LoadingScreenClass.IsValid())
	{
		LoadingScreenClass.LoadSynchronous();
	}

	if (!LoadingScreenClass.IsValid()) return;

	if (!LoadingScreenWidget)
	{
		LoadingScreenWidget = CreateWidget<UUserWidget>(this, LoadingScreenClass.Get());
	}

	if (LoadingScreenWidget && !LoadingScreenWidget->IsInViewport())
	{
		LoadingScreenWidget->AddToViewport(100);
		UE_LOG(LogTemp, Warning, TEXT("[Loading] Widget Added To Viewport"));
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void UEmberGameInstance::HideLoadingScreen()
{
	if (LoadingScreenWidget && LoadingScreenWidget->IsInViewport())
	{
		LoadingScreenWidget->RemoveFromParent();
	}
}

void UEmberGameInstance::RequestOpenLevel(FName MapName)
{
	ShowLoadingScreen();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, MapName]()
		{
			UGameplayStatics::OpenLevel(this, MapName);
		}, 0.8f, false);
}

void UEmberGameInstance::TestPlaySFX(ESfxSoundType SoundType, const FName RowName, FVector Location)
{
	AudioSubsystem->PlaySFX(SoundType, RowName, Location);
}
