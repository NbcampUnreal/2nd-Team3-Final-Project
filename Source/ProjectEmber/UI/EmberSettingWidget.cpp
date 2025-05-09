#include "EmberSettingWidget.h"
#include "Kismet/GameplayStatics.h"
//컨트롤러
#include "EmberMainMenuWidget.h"

void UEmberSettingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeUIOnly());
    }
}
