#include "EmberSettingWidget.h"
#include "EmberKeySettingWidget.h"
#include "GameInstance/EmberGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "EmberPauseWidget.h"
//컨트롤러
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "EmberMainMenuWidget.h"

void UEmberSettingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeUIOnly());
    }

    if (VideoButton)
    {
        VideoButton->OnClicked.AddDynamic(this, &UEmberSettingWidget::OnVideoButtonClicked);
    }

    if (AudioButton)
    {
        AudioButton->OnClicked.AddDynamic(this, &UEmberSettingWidget::OnAudioButtonClicked);
    }

    if (ControlButton)
    {
        ControlButton->OnClicked.AddDynamic(this, &UEmberSettingWidget::OnControlButtonClicked);
    }

    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &UEmberSettingWidget::OnBackButtonClicked);
    }
}

void UEmberSettingWidget::OnVideoButtonClicked()
{
    if (SettingsContentSwitcher)
    {
        SettingsContentSwitcher->SetActiveWidgetIndex(0);
    }
}

void UEmberSettingWidget::OnAudioButtonClicked()
{
    if (SettingsContentSwitcher)
    {
        SettingsContentSwitcher->SetActiveWidgetIndex(1);
    }
}

void UEmberSettingWidget::OnControlButtonClicked()
{
    if (SettingsContentSwitcher)
    {
        SettingsContentSwitcher->SetActiveWidgetIndex(2);

        if (UEmberKeySettingWidget* KeyWidget = Cast<UEmberKeySettingWidget>(SettingsContentSwitcher->GetWidgetAtIndex(2)))
        {
            if (UEmberGameInstance* GI = GetWorld()->GetGameInstance<UEmberGameInstance>())
            {
                KeyWidget->EditableIMC = GI->PlayerMappingContext;
            }
        }
    }
}

void UEmberSettingWidget::OnBackButtonClicked()
{
    RemoveFromParent();

    if (ParentPauseWidget)
    {
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            ParentPauseWidget->AddToViewport();
            PC->bShowMouseCursor = true;
            PC->SetInputMode(FInputModeUIOnly());
        }
    }
    else
    {
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            if (MainMenuWidgetClass)
            {
                UUserWidget* MainMenu = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass);
                if (MainMenu)
                {
                    MainMenu->AddToViewport();
                    PC->bShowMouseCursor = true;
                    PC->SetInputMode(FInputModeUIOnly());
                }
            }
        }
    }
}