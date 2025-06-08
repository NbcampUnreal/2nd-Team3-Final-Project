#include "Quest_WindowaboveWidget.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "UI/BaseWidget/GameMenuWidget.h"
#include "EmberUtils/FunctionLibrary/UIFunctionLibrary.h"
#include "UI/HUD/EmberMainHUD.h"

void UQuest_WindowaboveWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UQuest_WindowaboveWidget::OnCloseButtonClicked);
	}
}

void UQuest_WindowaboveWidget::OnCloseButtonClicked()
{
    if (UGameMenuWidget* GameMenu = GetTypedOuter<UGameMenuWidget>())
    {
        if (GameMenu->WidgetSwitcher && GameMenu->EmptyBorder)
        {
            GameMenu->WidgetSwitcher->SetActiveWidget(GameMenu->EmptyBorder);
        }
    }

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        UUIFunctionLibrary::FocusGame(PC); 
    }
}