#pragma once

#include "CoreMinimal.h"
#include "EnumSet.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelSubsystem.generated.h"

UCLASS()
class PROJECTEMBER_API ULevelSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	ELevelType GetCurrentLevel() const;
	void ResponseOpenLevel(ELevelType Type);
	void ConvertTypeToName(ELevelType Type);
	
private:
	UPROPERTY()
	ELevelType CurrentLevel = ELevelType::LobbyLevel;

	FName TargetName = "";

	static const FName LobbyLevelName;
	static const FName FieldLevelName;
};
