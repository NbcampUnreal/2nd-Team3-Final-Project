#include "GameInstance/EmberGameInstance.h"
#include "GameInstance/AudioSubsystem.h"
#include "GameInstance/LevelSubsystem.h"
#include "EasyMultiSave.h"
#include "EMSFunctionLibrary.h"
#include "GameInstance/EmberSaveGame.h"
#include "GameFramework/GameUserSettings.h" 
#include "UI/EmberKeySettingWidget.h"
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

    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        Settings->LoadSettings(true);
    }

    LoadKeyMappingsWithEMS();
}

void UEmberGameInstance::TestPlaySound()
{
	AudioSubsystem->PlayBGMSoundByArea(EAreaType::LobbyArea);
}

void UEmberGameInstance::RequestOpenLevel(FName MapName)
{
    UGameplayStatics::OpenLevel(this, MapName);
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

void UEmberGameInstance::ApplySavedActionKeyMappingsToUserSettings()
{
    if (SavedMappings.Num() == 0)
        return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    ULocalPlayer* LP = PC->GetLocalPlayer();
    if (!LP) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP);
    if (!Subsystem) return;

    UEnhancedInputUserSettings* UserSettings = Subsystem->GetUserSettings();
    if (!UserSettings) return;

    for (const auto& Entry : SavedMappings)
    {
        FMapPlayerKeyArgs Args;
        Args.MappingName = Entry.MappingName;
        Args.NewKey = Entry.BoundKey;
        Args.Slot = EPlayerMappableKeySlot::First;
        FGameplayTagContainer FailureReason;
        UserSettings->MapPlayerKey(Args, FailureReason);

        UE_LOG(LogTemp, Warning, TEXT("[KeyRemap] Apply: %s -> %s | Fail: %s"),
            *Entry.MappingName.ToString(),
            *Entry.BoundKey.ToString(),
            *FailureReason.ToString());
    }
    UserSettings->SaveSettings();

}

void UEmberGameInstance::SaveKeyMappingsWithEMS()
{
    UEmberSaveGame* SaveGameInstance = Cast<UEmberSaveGame>(
        UEMSFunctionLibrary::GetCustomSave(this, UEmberSaveGame::StaticClass(), TEXT("KeyRemapSlot"), TEXT(""))
    );
    if (!SaveGameInstance)
    {
        SaveGameInstance = NewObject<UEmberSaveGame>(this, UEmberSaveGame::StaticClass());
    }

    SaveGameInstance->SavedMappings = SavedMappings;
    SaveGameInstance->SavedMoveBindings = SavedMoveBindings;

    UEMSFunctionLibrary::SaveCustom(this, SaveGameInstance);
}

void UEmberGameInstance::LoadKeyMappingsWithEMS()
{
    UEmberSaveGame* LoadedSave = Cast<UEmberSaveGame>(
        UEMSFunctionLibrary::GetCustomSave(this, UEmberSaveGame::StaticClass(), TEXT("KeyRemapSlot"), TEXT(""))
    );
    if (LoadedSave)
    {
        SavedMappings = LoadedSave->SavedMappings;
        SavedMoveBindings = LoadedSave->SavedMoveBindings;
    }
}

void UEmberGameInstance::SaveVideoSettingsWithEMS(const FEmberVideoSettings& Settings)
{
    UEmberSaveGame* SaveGameInstance = Cast<UEmberSaveGame>(
        UEMSFunctionLibrary::GetCustomSave(this, UEmberSaveGame::StaticClass(), TEXT("VideoSettingSlot"), TEXT(""))
    );
    if (!SaveGameInstance)
    {
        SaveGameInstance = NewObject<UEmberSaveGame>(this, UEmberSaveGame::StaticClass());
    }

    SaveGameInstance->SavedVideoSettings = Settings;

    UEMSFunctionLibrary::SaveCustom(this, SaveGameInstance);
}

FEmberVideoSettings UEmberGameInstance::LoadVideoSettingsWithEMS()
{
    UEmberSaveGame* LoadedSave = Cast<UEmberSaveGame>(
        UEMSFunctionLibrary::GetCustomSave(this, UEmberSaveGame::StaticClass(), TEXT("VideoSettingSlot"), TEXT(""))
    );

    if (LoadedSave)
    {
        return LoadedSave->SavedVideoSettings;
    }

    return FEmberVideoSettings();
}

void UEmberGameInstance::SaveAudioSettingsWithEMS(const FEmberAudioSettings& Settings)
{
    UEmberSaveGame* SaveGameInstance = Cast<UEmberSaveGame>(
        UEMSFunctionLibrary::GetCustomSave(this, UEmberSaveGame::StaticClass(), TEXT("AudioSettingSlot"), TEXT(""))
    );
    if (!SaveGameInstance)
    {
        SaveGameInstance = NewObject<UEmberSaveGame>(this, UEmberSaveGame::StaticClass());
    }

    SaveGameInstance->SavedAudioSettings = Settings;
    UEMSFunctionLibrary::SaveCustom(this, SaveGameInstance);
}

FEmberAudioSettings UEmberGameInstance::LoadAudioSettingsWithEMS()
{
    UEmberSaveGame* Loaded = Cast<UEmberSaveGame>(
        UEMSFunctionLibrary::GetCustomSave(this, UEmberSaveGame::StaticClass(), TEXT("AudioSettingSlot"), TEXT(""))
    );

    if (Loaded)
    {
        return Loaded->SavedAudioSettings;
    }

    return FEmberAudioSettings();
}