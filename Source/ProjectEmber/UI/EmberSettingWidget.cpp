#include "EmberSettingWidget.h"
#include "Kismet/GameplayStatics.h"
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

void UEmberSettingWidget::OnBackButtonClicked()
{
    RemoveFromParent();

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
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MainMenuWidgetClass not set!"));
        }
    }
}