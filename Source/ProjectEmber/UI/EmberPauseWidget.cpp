#include "EmberPauseWidget.h"
#include "Components/Button.h"
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
	RemoveFromParent();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
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
		if (UUserWidget* SettingWidget = CreateWidget<UUserWidget>(PC, SettingWidgetClass))
		{
			RemoveFromParent();
			SettingWidget->AddToViewport();
		}
	}
}

void UEmberPauseWidget::OnQuitClicked()
{
	UGameplayStatics::OpenLevel(this, FName("TestMap"));
}