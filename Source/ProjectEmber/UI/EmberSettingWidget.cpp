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
}

void UEmberSettingWidget::OnVideoButtonClicked()
{
    if (SettingsContentSwitcher)
    {
        SettingsContentSwitcher->SetActiveWidgetIndex(0);
    }
}