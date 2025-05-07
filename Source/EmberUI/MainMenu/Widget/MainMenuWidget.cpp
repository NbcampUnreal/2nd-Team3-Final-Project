#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

bool UMainMenuWidget::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartClicked);
	}
	if (OptionsButton)
	{
		OptionsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOptionsClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
	}

	return true;
}

void UMainMenuWidget::OnStartClicked()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, TEXT("GameplayMap"));
	}
}

void UMainMenuWidget::OnOptionsClicked()
{
	// TODO: Show options menu
}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}
