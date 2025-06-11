#include "GameInstance/EmberGameInstance.h"
#include "GameInstance/AudioSubsystem.h"
#include "GameInstance/LevelSubsystem.h"
#include "UI/EmberKeySettingWidget.h"
#include "UI/EmberLoadingWidget.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Class.h"
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

        if (UEmberLoadingWidget* TypedLoading = Cast<UEmberLoadingWidget>(LoadingScreenWidget))
        {
            TypedLoading->Progress = 0.0f;
        }
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
		}, 30.0f, false);
}

void UEmberGameInstance::TestPlaySFX(ESfxSoundType SoundType, const FName RowName, FVector Location)
{
	AudioSubsystem->PlaySFX(SoundType, RowName, Location);
}

static FString GetMoveDirectionMappingName(EMoveDirection Dir)
{
    switch (Dir)
    {
    case EMoveDirection::Forward:  return TEXT("MoveForward");
    case EMoveDirection::Backward: return TEXT("MoveBackward");
    case EMoveDirection::Left:     return TEXT("MoveLeft");
    case EMoveDirection::Right:    return TEXT("MoveRight");
    default:                       return TEXT("");
    }
}

void UEmberGameInstance::ApplySavedMoveBindingsToUserSettings()
{
    if (!PlayerMappingContext || SavedMoveBindings.Num() == 0 || !MoveAction)
        return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("No PlayerController!"));
        return;
    }

    ULocalPlayer* LP = PC->GetLocalPlayer();
    if (!LP)
    {
        UE_LOG(LogTemp, Error, TEXT("No LocalPlayer!"));
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP);
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("No EnhancedInputLocalPlayerSubsystem!"));
        return;
    }

    UEnhancedInputUserSettings* UserSettings = Subsystem->GetUserSettings();
    if (!UserSettings)
    {
        UE_LOG(LogTemp, Error, TEXT("No EnhancedInputUserSettings!"));
        return;
    }

    for (const auto& MoveEntry : SavedMoveBindings)
    {
        FMapPlayerKeyArgs Args;
        Args.MappingName = FName(GetMoveDirectionMappingName(MoveEntry.Direction));
        Args.Slot = EPlayerMappableKeySlot::First;
        Args.NewKey = MoveEntry.BoundKey;

        FGameplayTagContainer FailureReason;
        UserSettings->MapPlayerKey(Args, FailureReason);

        if (!FailureReason.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("Mapping failed: %s (Key: %s)"), *FailureReason.ToString(), *Args.NewKey.ToString());
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("=== UserSettings mapping complete ==="));
}