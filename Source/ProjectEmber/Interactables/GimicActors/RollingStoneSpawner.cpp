#include "Interactables/GimicActors/RollingStoneSpawner.h"
#include "Interactables/GimicActors/RollingStone.h"

ARollingStoneSpawner::ARollingStoneSpawner()
{
 	PrimaryActorTick.bCanEverTick = false;

}

void ARollingStoneSpawner::BeginPlay()
{
	Super::BeginPlay();
	// 테스트용
	SpawnRollingStone();
}

void ARollingStoneSpawner::SetIsSpawning(const bool isSpawning)
{
	bIsSpawning = isSpawning;
}

bool ARollingStoneSpawner::GetIsSpawning() const
{
	return bIsSpawning;
}

void ARollingStoneSpawner::SpawnRollingStone()
{
	if (!RollingStone && RollingStoneClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;                           // 소유자 지정 (옵션)
		SpawnParams.Instigator = GetInstigator();           // 인스티게이터 지정 (옵션)
		SpawnParams.SpawnCollisionHandlingOverride =        // 충돌 처리 방식
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
		RollingStone = GetWorld()->SpawnActor<ARollingStone>(RollingStoneClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
		bIsSpawning = true;
	}
}

void ARollingStoneSpawner::DestroyRollingStone()
{
	if (IsValid(RollingStone))
	{
		RollingStone->Destroy();
		RollingStone = nullptr;
		
		bIsSpawning = false;
	}
}

