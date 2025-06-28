#include "GameInstance/DungeonSubsystem.h"
#include "DungeonGenerator.h"
#include "Door.h"

void UDungeonSubsystem::SetTempDungeonGenerator(ADungeonGenerator* DungeonGenerator)
{
	if (IsValid(DungeonGenerator))
	{
		TempDungeonGenerator = DungeonGenerator;
	}
}

ADungeonGenerator* UDungeonSubsystem::GetTempDungeonGenerator()
{
	if (IsValid(TempDungeonGenerator))
	{
		return TempDungeonGenerator.Get();
	}

	return nullptr;
}

void UDungeonSubsystem::SetTempBossDoor(ADoor* BossDoor)
{
	if (IsValid(BossDoor))
	{
		TempBossDoor = BossDoor;
	}
}

ADoor* UDungeonSubsystem::GetTempBossDoor()
{
	if (IsValid(TempBossDoor))
	{
		return TempBossDoor.Get();
	}

	return nullptr;
}
