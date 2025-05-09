#include "Interactables/MiningActor.h"

void AMiningActor::Interact_Implementation(AActor* Caller)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;                           // 소유자 지정 (옵션)
	SpawnParams.Instigator = GetInstigator();           // 인스티게이터 지정 (옵션)
	SpawnParams.SpawnCollisionHandlingOverride =        // 충돌 처리 방식
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const int32_t RandomInt = FMath::RandRange(2, 5);
	const FVector Location = this->GetActorLocation();

	for (int32_t i = 0; i < RandomInt; i++)
	{
		const float Angle = 2 * PI * i / RandomInt;
		const float Radius = 50.f;  // 반경 50cm
		FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0) * Radius;
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnActorClass, Location + Offset, FRotator::ZeroRotator, SpawnParams);
		SpawnedActor->SetActorEnableCollision(false);
		
		FTimerHandle TimerHandle;

		FTimerDelegate Delegate;
		Delegate.BindLambda([SpawnedActor]()
		{
			if (IsValid(SpawnedActor))
			{
				SpawnedActor->SetActorEnableCollision(true);
			}
		});
		
		GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		Delegate,
		0.1f,
		false
		);
		
	}
	Destroy();
}

float AMiningActor::GetGatherTime_Implementation()
{
	return GatherTime;
}
