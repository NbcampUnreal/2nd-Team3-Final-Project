#include "EmberVideoSettingPanel.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
#include "GameInstance/EmberGameInstance.h"
#include "GameInstance/EmberSaveGame.h"
#include "GameInstance/EmberVideoSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void UEmberVideoSettingPanel::NativeConstruct()
{
    Super::NativeConstruct();

    InitOptions();

    if (ApplyButton)
    {
        ApplyButton->OnClicked.AddDynamic(this, &UEmberVideoSettingPanel::ApplyVideoSettings);
    }

    if (OverallComboBox)
    {
        OverallComboBox->OnSelectionChanged.AddDynamic(this, &UEmberVideoSettingPanel::OnOverallSettingChanged);
    }
}

void UEmberVideoSettingPanel::InitOptions()
{
    if (!GEngine) return;

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    if (!Settings) return;

    FEmberVideoSettings Loaded;
    if (UEmberGameInstance* GI = GetWorld()->GetGameInstance<UEmberGameInstance>())
    {
        Loaded = GI->LoadVideoSettingsWithEMS();
    }


    if (ScreenModeComboBox)
    {
        ScreenModeComboBox->ClearOptions();
        ScreenModeComboBox->AddOption(TEXT("Fullscreen"));
        ScreenModeComboBox->AddOption(TEXT("Windowed"));
        ScreenModeComboBox->AddOption(TEXT("Borderless"));

        if (!Loaded.ScreenMode.IsEmpty())
        {
            ScreenModeComboBox->SetSelectedOption(Loaded.ScreenMode);
        }
        else
        {
            switch (Settings->GetFullscreenMode())
            {
            case EWindowMode::Fullscreen:
                ScreenModeComboBox->SetSelectedOption(TEXT("Fullscreen"));
                break;
            case EWindowMode::Windowed:
                ScreenModeComboBox->SetSelectedOption(TEXT("Windowed"));
                break;
            case EWindowMode::WindowedFullscreen:
                ScreenModeComboBox->SetSelectedOption(TEXT("Borderless"));
                break;
            }
        }
    }

    if (ResolutionComboBox)
    {
        ResolutionComboBox->ClearOptions();
        ResolutionComboBox->AddOption(TEXT("1920x1080"));
        ResolutionComboBox->AddOption(TEXT("1280x720"));
        ResolutionComboBox->AddOption(TEXT("600x480"));

        if (!Loaded.Resolution.IsEmpty())
        {
            ResolutionComboBox->SetSelectedOption(Loaded.Resolution);
        }
        else
        {
            FIntPoint Res = Settings->GetScreenResolution();
            FString ResString = FString::Printf(TEXT("%dx%d"), Res.X, Res.Y);
            ResolutionComboBox->SetSelectedOption(ResString);
        }
    }

    TArray<FString> QualityOptions = { TEXT("Low"), TEXT("Medium"), TEXT("High"), TEXT("Epic") };
    auto SetupQualityCombo = [&](UComboBoxString* Combo, const FString& Value, int32 DefaultIndex)
    {
        if (!Combo) return;
        Combo->ClearOptions();
        for (const auto& Opt : QualityOptions)
        {
            Combo->AddOption(Opt);
        }
        if (!Value.IsEmpty())
        {
            Combo->SetSelectedOption(Value);
        }
        else
        {
            Combo->SetSelectedIndex(DefaultIndex);
        }
    };

    SetupQualityCombo(OverallComboBox, Loaded.OverallQuality, Settings->GetOverallScalabilityLevel());
    SetupQualityCombo(TextureComboBox, Loaded.TextureQuality, Settings->ScalabilityQuality.TextureQuality);
    SetupQualityCombo(ShadowComboBox, Loaded.ShadowQuality, Settings->ScalabilityQuality.ShadowQuality);
    SetupQualityCombo(EffectsComboBox, Loaded.EffectsQuality, Settings->ScalabilityQuality.EffectsQuality);
    SetupQualityCombo(AAComboBox, Loaded.AntiAliasingQuality, Settings->ScalabilityQuality.AntiAliasingQuality);
    SetupQualityCombo(ViewDistanceComboBox, Loaded.ViewDistanceQuality, Settings->ScalabilityQuality.ViewDistanceQuality);
}

int32 UEmberVideoSettingPanel::GetQualityIndexFromString(const FString& QualityName) const
{
    if (QualityName == TEXT("Low")) return 0;
    if (QualityName == TEXT("Medium")) return 1;
    if (QualityName == TEXT("High")) return 2;
    if (QualityName == TEXT("Epic")) return 3;
    return 3;
}

void UEmberVideoSettingPanel::ApplyVideoSettings()
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        FString ResString = ResolutionComboBox->GetSelectedOption();
        FString Left, Right;
        if (ResString.Split(TEXT("x"), &Left, &Right))
        {
            int32 Width = FCString::Atoi(*Left);
            int32 Height = FCString::Atoi(*Right);
            Settings->SetScreenResolution(FIntPoint(Width, Height));
        }

        FString Mode = ScreenModeComboBox->GetSelectedOption();
        if (Mode == TEXT("Fullscreen"))
            Settings->SetFullscreenMode(EWindowMode::Fullscreen);
        else if (Mode == TEXT("Windowed"))
            Settings->SetFullscreenMode(EWindowMode::Windowed);
        else
            Settings->SetFullscreenMode(EWindowMode::WindowedFullscreen);

        Settings->SetOverallScalabilityLevel(GetQualityIndexFromString(OverallComboBox->GetSelectedOption()));
        Settings->ScalabilityQuality.TextureQuality = GetQualityIndexFromString(TextureComboBox->GetSelectedOption());
        Settings->ScalabilityQuality.ShadowQuality = GetQualityIndexFromString(ShadowComboBox->GetSelectedOption());
        Settings->ScalabilityQuality.EffectsQuality = GetQualityIndexFromString(EffectsComboBox->GetSelectedOption());
        Settings->ScalabilityQuality.AntiAliasingQuality = GetQualityIndexFromString(AAComboBox->GetSelectedOption());
        Settings->ScalabilityQuality.ViewDistanceQuality = GetQualityIndexFromString(ViewDistanceComboBox->GetSelectedOption());

        Settings->ApplySettings(false);
        Settings->SaveSettings();
    }

    if (UEmberGameInstance* GI = GetWorld()->GetGameInstance<UEmberGameInstance>())
    {
        FEmberVideoSettings Data;
        Data.Resolution = ResolutionComboBox->GetSelectedOption();
        Data.ScreenMode = ScreenModeComboBox->GetSelectedOption();
        Data.OverallQuality = OverallComboBox->GetSelectedOption();
        Data.TextureQuality = TextureComboBox->GetSelectedOption();
        Data.ShadowQuality = ShadowComboBox->GetSelectedOption();
        Data.EffectsQuality = EffectsComboBox->GetSelectedOption();
        Data.AntiAliasingQuality = AAComboBox->GetSelectedOption();
        Data.ViewDistanceQuality = ViewDistanceComboBox->GetSelectedOption();

        GI->SaveVideoSettingsWithEMS(Data);
    }
}

UFUNCTION()
void UEmberVideoSettingPanel::OnOverallSettingChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (!SelectedItem.IsEmpty())
    {
        if (TextureComboBox) TextureComboBox->SetSelectedOption(SelectedItem);
        if (ShadowComboBox) ShadowComboBox->SetSelectedOption(SelectedItem);
        if (EffectsComboBox) EffectsComboBox->SetSelectedOption(SelectedItem);
        if (AAComboBox) AAComboBox->SetSelectedOption(SelectedItem);
        if (ViewDistanceComboBox) ViewDistanceComboBox->SetSelectedOption(SelectedItem);
    }
}