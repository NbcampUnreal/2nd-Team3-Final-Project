#include "EmberVideoSettingPanel.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
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
}

void UEmberVideoSettingPanel::InitOptions()
{
    if (!GEngine) return;

    // Screen Modes
    if (ScreenModeComboBox)
    {
        ScreenModeComboBox->ClearOptions();
        ScreenModeComboBox->AddOption(TEXT("Fullscreen"));
        ScreenModeComboBox->AddOption(TEXT("Windowed"));
        ScreenModeComboBox->AddOption(TEXT("Borderless"));
        ScreenModeComboBox->SetSelectedIndex(0);
    }

    // Common Resolutions (you can dynamically detect them too)
    if (ResolutionComboBox)
    {
        ResolutionComboBox->ClearOptions();
        ResolutionComboBox->AddOption(TEXT("1920x1080"));
        ResolutionComboBox->AddOption(TEXT("1280x720"));
        ResolutionComboBox->AddOption(TEXT("600x480"));
        ResolutionComboBox->SetSelectedIndex(0);
    }

    TArray<FString> QualityOptions = { TEXT("Low"), TEXT("Medium"), TEXT("High"), TEXT("Epic") };

    auto SetupQualityCombo = [&](UComboBoxString* Combo) {
        if (!Combo) return;
        Combo->ClearOptions();
        for (auto& Opt : QualityOptions)
        {
            Combo->AddOption(Opt);
        }
        Combo->SetSelectedIndex(3); // Default to Epic
        };

    SetupQualityCombo(OverallComboBox);
    SetupQualityCombo(TextureComboBox);
    SetupQualityCombo(ShadowComboBox);
    SetupQualityCombo(EffectsComboBox);
    SetupQualityCombo(AAComboBox);
    SetupQualityCombo(ViewDistanceComboBox);
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
        // Resolution
        FString ResString = ResolutionComboBox->GetSelectedOption();
        FString Left, Right;
        if (ResString.Split(TEXT("x"), &Left, &Right))
        {
            int32 Width = FCString::Atoi(*Left);
            int32 Height = FCString::Atoi(*Right);
            Settings->SetScreenResolution(FIntPoint(Width, Height));
        }

        // Window Mode
        FString Mode = ScreenModeComboBox->GetSelectedOption();
        if (Mode == TEXT("Fullscreen"))
            Settings->SetFullscreenMode(EWindowMode::Fullscreen);
        else if (Mode == TEXT("Windowed"))
            Settings->SetFullscreenMode(EWindowMode::Windowed);
        else
            Settings->SetFullscreenMode(EWindowMode::WindowedFullscreen);

        // Scalability
        Settings->SetOverallScalabilityLevel(GetQualityIndexFromString(OverallComboBox->GetSelectedOption()));
        Settings->ScalabilityQuality.TextureQuality = GetQualityIndexFromString(TextureComboBox->GetSelectedOption());
        Settings->ScalabilityQuality.ShadowQuality = GetQualityIndexFromString(ShadowComboBox->GetSelectedOption());
        Settings->ScalabilityQuality.EffectsQuality = GetQualityIndexFromString(EffectsComboBox->GetSelectedOption());
        Settings->ScalabilityQuality.AntiAliasingQuality = GetQualityIndexFromString(AAComboBox->GetSelectedOption());
        Settings->ScalabilityQuality.ViewDistanceQuality = GetQualityIndexFromString(ViewDistanceComboBox->GetSelectedOption());

        Settings->ApplySettings(false);
        Settings->SaveSettings();
    }
}