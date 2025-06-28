#include "EmberUIButton.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/AudioSubsystem.h"
#include "GameInstance/EmberGameInstance.h"

void UEmberUIButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	OnHovered.AddUniqueDynamic(this, &UEmberUIButton::OnHoveredInternal);
	OnClicked.AddUniqueDynamic(this, &UEmberUIButton::OnClickedInternal);
}

void UEmberUIButton::PostLoad()
{
	Super::PostLoad();

	OnHovered.AddUniqueDynamic(this, &UEmberUIButton::OnHoveredInternal);
	OnClicked.AddUniqueDynamic(this, &UEmberUIButton::OnClickedInternal);
}

void UEmberUIButton::OnHoveredInternal()
{
	PlayUISound(HoverSound);
}

void UEmberUIButton::OnClickedInternal()
{
	PlayUISound(ClickSound);
}

void UEmberUIButton::PlayUISound(USoundBase* Sound)
{
	if (!Sound) return;

	if (const UWorld* World = GetWorld())
	{
		if (const UEmberGameInstance* GameInstance = Cast<UEmberGameInstance>(World->GetGameInstance()))
		{
			if (const UAudioSubsystem* Audio = GameInstance->GetSubsystem<UAudioSubsystem>())
			{
				const float Volume = Audio->GetUIVolume() * Audio->GetMasterVolume();
				UGameplayStatics::PlaySound2D(this, Sound, Volume);
			}
		}
	}
}