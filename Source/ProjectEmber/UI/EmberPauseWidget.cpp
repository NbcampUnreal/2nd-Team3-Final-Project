#include "EmberPauseWidget.h"
#include "EmberSettingWidget.h"
#include "GameplayTagContainer.h"
#include "BaseWidget/GameMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "FunctionLibrary/UIFunctionLibrary.h"

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

void UEmberPauseWidget::SetParentGameMenuWidget(UUserWidget* InParentGameMenuWidget)
{
	ParentGameMenuWidget = InParentGameMenuWidget;
}

void UEmberPauseWidget::OnContinueClicked()
{
	if (ParentGameMenuWidget)
	{
		Cast<UGameMenuWidget>(ParentGameMenuWidget)->Input_TogglePause();
		UUIFunctionLibrary::FocusGame(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
	}
}

void UEmberPauseWidget::OnSettingsClicked()
{
	if (!SettingWidgetClass) return;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		Cast<UGameMenuWidget>(ParentGameMenuWidget)->Input_TogglePause();
		
		UUserWidget* SettingsWidgetInstance = UUIFunctionLibrary::PushContentToLayer(PC, FGameplayTag::RequestGameplayTag("UI.Layer.Modal"), SettingWidgetClass);
		Cast<UEmberSettingWidget>(SettingsWidgetInstance)->ParentPauseWidget = this;
		UUIFunctionLibrary::FocusUI(PC,SettingsWidgetInstance,true,true,true);
	}
}

void UEmberPauseWidget::OnQuitClicked()
{
	UGameplayStatics::OpenLevel(this, FName("TestMap"));
}