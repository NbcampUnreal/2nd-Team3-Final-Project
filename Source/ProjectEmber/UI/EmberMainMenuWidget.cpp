#include "EmberMainMenuWidget.h"
#include "EmberSaveWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "EmberSettingWidget.h"
#include "GameInstance/EmberGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UEmberMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (ContinueButton)
        ContinueButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnContinueClicked);

    if (SettingsButton)
        SettingsButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnSettingsClicked);

    if (QuitButton)
        QuitButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnQuitClicked);
}

void UEmberMainMenuWidget::OnContinueClicked()
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (SaveWidgetClass)
        {
            if (UUserWidget* SaveWidget = CreateWidget<UEmberSaveWidget>(PC, SaveWidgetClass))
            {
                RemoveFromParent();

                SaveWidget->AddToViewport();

                PC->bShowMouseCursor = true;
                PC->SetInputMode(FInputModeUIOnly());
            }
        }
    }
}

void UEmberMainMenuWidget::OnSettingsClicked()
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (SettingWidgetClass)
        {
            if (UEmberSettingWidget* Settings = CreateWidget<UEmberSettingWidget>(PC, SettingWidgetClass))
            {
                RemoveFromParent();

                Settings->AddToViewport();

                PC->bShowMouseCursor = true;
                PC->SetInputMode(FInputModeUIOnly());
            }
        }
    }
}

void UEmberMainMenuWidget::OnQuitClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}