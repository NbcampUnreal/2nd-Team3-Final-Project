#include "EmberMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "EmberSettingWidget.h"
#include "GameInstance/EmberGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UEmberMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (NewGameButton)
        NewGameButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnNewGameClicked);

    if (ContinueButton)
        ContinueButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnContinueClicked);

    if (SettingsButton)
        SettingsButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnSettingsClicked);

    if (QuitButton)
        QuitButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnQuitClicked);
}

void UEmberMainMenuWidget::OnNewGameClicked()
{
    RemoveFromParent();

    if (UEmberGameInstance* GI = Cast<UEmberGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GI->RequestOpenLevel("L_Als_Playground");
    }
}

void UEmberMainMenuWidget::OnContinueClicked()
{
    this->RemoveFromParent();
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