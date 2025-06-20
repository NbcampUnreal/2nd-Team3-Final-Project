#include "EmberAudioSettingWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameInstance/AudioSubsystem.h"
#include "GameInstance/EmberGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UEmberAudioSettingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    float Master = 1.f, Bgm = 1.f, Effects = 1.f;

    if (UEmberGameInstance* GI = Cast<UEmberGameInstance>(GetGameInstance()))
    {
        FEmberAudioSettings Loaded = GI->LoadAudioSettingsWithEMS();
        Master = Loaded.MasterVolume;
        Bgm = Loaded.BgmVolume;
        Effects = Loaded.EffectsVolume;
    }

    MasterSlider->SetValue(Master);
    BgmSlider->SetValue(Bgm);
    EffectsSlider->SetValue(Effects);

    OnSliderChanged(Master, MasterValueText);
    OnSliderChanged(Bgm, BgmValueText);
    OnSliderChanged(Effects, EffectsValueText);

    MasterSlider->OnValueChanged.AddDynamic(this, &UEmberAudioSettingWidget::OnMasterSliderChanged);
    BgmSlider->OnValueChanged.AddDynamic(this, &UEmberAudioSettingWidget::OnBgmSliderChanged);
    EffectsSlider->OnValueChanged.AddDynamic(this, &UEmberAudioSettingWidget::OnEffectsSliderChanged);
    ApplyButton->OnClicked.AddDynamic(this, &UEmberAudioSettingWidget::OnApplyClicked);
}

void UEmberAudioSettingWidget::OnSliderChanged(float Value, UTextBlock* TargetText)
{
    if (TargetText)
    {
        const int32 IntVal = FMath::RoundToInt(Value * 100.f);
        TargetText->SetText(FText::AsNumber(IntVal));
    }
}

void UEmberAudioSettingWidget::OnMasterSliderChanged(float Value)
{
    OnSliderChanged(Value, MasterValueText);
}

void UEmberAudioSettingWidget::OnBgmSliderChanged(float Value)
{
    OnSliderChanged(Value, BgmValueText);
}
void UEmberAudioSettingWidget::OnEffectsSliderChanged(float Value)
{
    OnSliderChanged(Value, EffectsValueText);
}


void UEmberAudioSettingWidget::OnApplyClicked()
{
    if (UAudioSubsystem* Audio = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
    {
        float Master = MasterSlider->GetValue();
        float Bgm = BgmSlider->GetValue();
        float Effects = EffectsSlider->GetValue();

        Audio->SetAndApplyMasterVolume(Master);
        Audio->SetBgmVolume(Bgm);
        Audio->SetEffectsVolume(Effects);

        if (UEmberGameInstance* GI = Cast<UEmberGameInstance>(GetGameInstance()))
        {
            FEmberAudioSettings AudioSettings;
            AudioSettings.MasterVolume = Master;
            AudioSettings.BgmVolume = Bgm;
            AudioSettings.EffectsVolume = Effects;

            GI->SaveAudioSettingsWithEMS(AudioSettings);
        }
    }
}
