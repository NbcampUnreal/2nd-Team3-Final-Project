#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmberAudioSettingWidget.generated.h"

class USlider;
class UTextBlock;
class UButton;

UCLASS()
class PROJECTEMBER_API UEmberAudioSettingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnApplyClicked();

    UFUNCTION()
    void OnSliderChanged(float Value, UTextBlock* TargetText);

    UFUNCTION()
    void OnMasterSliderChanged(float Value);

    UFUNCTION()
    void OnBgmSliderChanged(float Value);

    UFUNCTION()
    void OnEffectsSliderChanged(float Value);

    UFUNCTION()
    void OnUISliderChanged(float Value);

    UPROPERTY(meta = (BindWidget))
    USlider* MasterSlider;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* MasterValueText;

    UPROPERTY(meta = (BindWidget))
    USlider* BgmSlider;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BgmValueText;

    UPROPERTY(meta = (BindWidget))
    USlider* EffectsSlider;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EffectsValueText;

    UPROPERTY(meta = (BindWidget))
    USlider* UISlider;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* UIValueText;

    UPROPERTY(meta = (BindWidget))
    UButton* ApplyButton;
	
};
