#include "LevelSubsystem.h"
#include "Kismet/GameplayStatics.h"

const FName ULevelSubsystem::LobbyLevelName = "LobbyLevel";
const FName ULevelSubsystem::FieldLevelName = "FieldLevel";

ELevelType ULevelSubsystem::GetCurrentLevel() const
{
	return CurrentLevel;
}

void ULevelSubsystem::ResponseOpenLevel(ELevelType Type)
{
	CurrentLevel = Type;

	ConvertTypeToName(Type);

	if (IsValid(GetWorld()))
	{
		UGameplayStatics::OpenLevel(GetWorld(), TargetName);
	}
}

void ULevelSubsystem::ConvertTypeToName(ELevelType Type)
{
	switch (Type)
	{
	case ELevelType::LobbyLevel:
		TargetName = LobbyLevelName;
		break;
	case ELevelType::FieldLevel:
		TargetName = FieldLevelName;
		break;
	}
}
