#include "EmberMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "EmberSettingWidget.h"

void UEmberMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UButton* NewGameButton = Cast<UButton>(GetWidgetFromName(TEXT("NewGameButton"))))
		NewGameButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnNewGameClicked);

    if (UButton* ContinueButton = Cast<UButton>(GetWidgetFromName(TEXT("ContinueButton"))))
        ContinueButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnContinueClicked);

    if (UButton* SettingsButton = Cast<UButton>(GetWidgetFromName(TEXT("SettingsButton"))))
        SettingsButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnSettingsClicked);

    if (UButton* QuitButton = Cast<UButton>(GetWidgetFromName(TEXT("QuitButton"))))
        QuitButton->OnClicked.AddDynamic(this, &UEmberMainMenuWidget::OnQuitClicked);
}

void UEmberMainMenuWidget::OnNewGameClicked()
{
    RemoveFromParent();

    FTimerHandle TempHandle;
    GetWorld()->GetTimerManager().SetTimer(TempHandle, [this]()
        {
            UGameplayStatics::OpenLevel(this, FName("L_Als_Playground"));
        }, 0.1f, false);
}

void UEmberMainMenuWidget::OnContinueClicked()
{
    this->RemoveFromParent();
}

void UEmberMainMenuWidget::OnSettingsClicked()
{
    this->RemoveFromParent();

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (auto* Settings = CreateWidget<UEmberSettingWidget>(PC, UEmberSettingWidget::StaticClass()))
        {
            this->RemoveFromParent();
            Settings->AddToViewport();
        }
    }
}

void UEmberMainMenuWidget::OnQuitClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}