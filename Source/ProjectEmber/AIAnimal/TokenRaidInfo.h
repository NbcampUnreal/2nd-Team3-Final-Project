#pragma once
#include "CoreMinimal.h"
#include "BaseAIAnimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"

#include "TokenRaidInfo.generated.h"

USTRUCT(BlueprintType)
struct PROJECTEMBER_API FTokenRaidInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 지역 이름 -> UI 연동에 좋을 거 같아서 추가함
	UPROPERTY(EditAnywhere, Category = "RaidSpawn")
	FGameplayTag Region;

	// 난이도 (Easy, Normal, Hard)->UI 연동에 좋을 거 같아서 추가함
	UPROPERTY(EditAnywhere, Category = "RaidSpawn")
	FGameplayTag Difficulty;

	// 웨이브 번호 (0부터 시작)
	UPROPERTY(EditAnywhere, Category = "RaidSpawn")
	int32 Wave = 0;

	// 웨이브 당 그룹 수 (이 웨이브에 몇 개 그룹이 등장할지)
	UPROPERTY(EditAnywhere, Category = "RaidSpawn")
	int32 GroupsPerWave = 0;

	// 그룹당 수
	UPROPERTY(EditAnywhere, Category = "RaidSpawn")
	int32 UnitsPerGroup = 0;

	// 스폰 가능한 객체들
	UPROPERTY(EditAnywhere, Category = "RaidSpawn")
	TSubclassOf<ABaseAIAnimal> AnimalClass;
};
