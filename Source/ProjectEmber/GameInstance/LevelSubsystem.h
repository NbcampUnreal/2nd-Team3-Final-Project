#pragma once

#include "CoreMinimal.h"
#include "EnumSet.h"
#include "WorldPartition/WorldPartitionSubsystem.h"
#include "WorldPartition/WorldPartitionStreamingSource.h"
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
	void OnStreamingStateUpdated();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
private:
	UPROPERTY()
	ELevelType CurrentLevel = ELevelType::LobbyLevel;

	FName TargetName = "";

	FVector TeleportTargetLocation = FVector::ZeroVector;

	static const FName LobbyLevelName;
	static const FName FieldLevelName;
};
