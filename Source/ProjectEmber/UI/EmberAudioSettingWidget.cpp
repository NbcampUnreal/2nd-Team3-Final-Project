#include "EmberAudioSettingWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameInstance/AudioSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UEmberAudioSettingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (UAudioSubsystem* Audio = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
    {
        MasterSlider->SetValue(Audio->GetMasterVolume());
        BgmSlider->SetValue(Audio->GetBgmVolume());
        EffectsSlider->SetValue(Audio->GetEffectsVolume());

        OnSliderChanged(MasterSlider->GetValue(), MasterValueText);
        OnSliderChanged(BgmSlider->GetValue(), BgmValueText);
        OnSliderChanged(EffectsSlider->GetValue(), EffectsValueText);
    }

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
        Audio->SetAndApplyMasterVolume(MasterSlider->GetValue());
        Audio->SetBgmVolume(BgmSlider->GetValue());
        Audio->SetEffectsVolume(EffectsSlider->GetValue());
        // Easy multi save
    }
}
