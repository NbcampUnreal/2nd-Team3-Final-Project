#include "LevelSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "WorldPartition/WorldPartitionRuntimeCell.h"

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

void ULevelSubsystem::OnStreamingStateUpdated()
{
	// 예시: 특정 위치/반경에 대한 QuerySource 설정
	TArray<FWorldPartitionStreamingQuerySource> QuerySources;
	FWorldPartitionStreamingQuerySource Source;
	Source.bSpatialQuery = true;
	Source.Location = TeleportTargetLocation/* 원하시는 위치 */;
	Source.Rotation = FRotator::ZeroRotator/* 회전 (보통 FRotator::ZeroRotator) */;
	Source.TargetGrids = { NAME_None };
	Source.bUseGridLoadingRange = false;
	QuerySources.Add(Source);

	// 스트리밍이 완료되었는지 확인
	if (UWorldPartitionSubsystem* WPSS = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>())
	{
		
		if (WPSS->IsStreamingCompleted(EWorldPartitionRuntimeCellState::Activated, QuerySources, /*bExactState=*/false))
		{
			UE_LOG(LogTemp, Log, TEXT("Partition Cell 로딩 완료!"));
			// 후속 로직 호출
		}
	}
}

void ULevelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UWorldPartitionSubsystem* WPSS = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>())
	{
		// 델리게이트 등록 (인자 없는 콜백)
		WPSS->OnStreamingStateUpdated().AddUObject(this, &ULevelSubsystem::OnStreamingStateUpdated);
	}
}
