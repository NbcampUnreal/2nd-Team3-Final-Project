#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmberSettingWidget.generated.h"

class UButton;
class UWidgetSwitcher;

UCLASS()
class PROJECTEMBER_API UEmberSettingWidget : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnVideoButtonClicked();

    UPROPERTY(meta = (BindWidget))
    UButton* VideoButton;

    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* SettingsContentSwitcher;
};