#include "EmberSettingWidget.h"
#include "Kismet/GameplayStatics.h"
//��Ʈ�ѷ�
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
