#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputMappingContext.h"
#include "EmberSettingWidget.generated.h"

class UButton;
class UWidgetSwitcher;
class UEmberPauseWidget;

UCLASS()
class PROJECTEMBER_API UEmberSettingWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY()
    UEmberPauseWidget* ParentPauseWidget = nullptr;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnVideoButtonClicked();

    UFUNCTION()
    void OnAudioButtonClicked();

    UFUNCTION()
    void OnControlButtonClicked();

    UFUNCTION()
    void OnBackButtonClicked();

    UPROPERTY(meta = (BindWidget)) 
    UButton* VideoButton;

    UPROPERTY(meta = (BindWidget))
    UButton* AudioButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ControlButton;

    UPROPERTY(meta = (BindWidget))
    UButton* BackButton;

    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* SettingsContentSwitcher;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> MainMenuWidgetClass;
};