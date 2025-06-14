#include "EmberPauseWidget.h"
#include "EmberSettingWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void UEmberPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UEmberPauseWidget::OnContinueClicked);
	}

	if (SettingButton)
	{
		SettingButton->OnClicked.AddDynamic(this, &UEmberPauseWidget::OnSettingsClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UEmberPauseWidget::OnQuitClicked);
	}
}

void UEmberPauseWidget::OnContinueClicked()
{
	if (ParentGameMenuWidget)
	{
		if (UWidgetSwitcher* Switcher = Cast<UWidgetSwitcher>(
			ParentGameMenuWidget->GetWidgetFromName(TEXT("WidgetSwitcher"))))
		{
			Switcher->SetActiveWidgetIndex(0);
		}
	}

	this->SetVisibility(ESlateVisibility::Collapsed);

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetPause(false);
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

void UEmberPauseWidget::OnSettingsClicked()
{
	if (!SettingWidgetClass) return;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UEmberSettingWidget* Settings = CreateWidget<UEmberSettingWidget>(PC, SettingWidgetClass))
		{
			Settings->ParentPauseWidget = this;
			RemoveFromParent();
			Settings->AddToViewport();
		}
	}
}

void UEmberPauseWidget::OnQuitClicked()
{
	UGameplayStatics::OpenLevel(this, FName("TestMap"));
}