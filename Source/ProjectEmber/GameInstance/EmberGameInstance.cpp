#include "GameInstance/EmberGameInstance.h"
#include "GameInstance/AudioSubsystem.h"
#include "GameInstance/LevelSubsystem.h"

void UEmberGameInstance::Init()
{
	Super::Init();

	AudioSubsystem = GetSubsystem<UAudioSubsystem>();
	LevelSubsystem = GetSubsystem<ULevelSubsystem>();
}

void UEmberGameInstance::TestPlaySound()
{
	AudioSubsystem->PlayBGMSoundByArea(EAreaType::LobbyArea);
}
