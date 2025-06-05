#include "EmberSaveWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UEmberSaveWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BTN_Back)
    {
        BTN_Back->OnClicked.AddDynamic(this, &UEmberSaveWidget::OnBackClicked);
    }
}

void UEmberSaveWidget::OnBackClicked()
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (MainMenuWidgetClass)
        {
            if (UUserWidget* MainMenu = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass))
            {
                RemoveFromParent();
                MainMenu->AddToViewport();

                PC->bShowMouseCursor = true;
                PC->SetInputMode(FInputModeUIOnly());
            }
        }
    }
}