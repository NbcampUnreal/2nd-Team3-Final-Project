#include "Interactables/GimicActors/RollingStoneDestroyer.h"
#include "Interactables/GimicActors/RollingStoneSpawner.h"
#include "Interactables/GimicActors/RollingStone.h"

#include "Components/BoxComponent.h"

ARollingStoneDestroyer::ARollingStoneDestroyer()
{
	PrimaryActorTick.bCanEverTick = false;

	DestroyerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DestroyerBox"));
	DestroyerBox->InitBoxExtent(FVector(100.f));
	DestroyerBox->SetCollisionProfileName(TEXT("Trigger"));

	DestroyerBox->OnComponentBeginOverlap.AddDynamic(this, &ARollingStoneDestroyer::OnOverlapBegin);
}

void ARollingStoneDestroyer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ARollingStone* Actor = Cast<ARollingStone>(OtherActor))
	{
		Spawner->DestroyRollingStone();
		Spawner->SpawnRollingStone();
	}
}

void ARollingStoneDestroyer::BeginPlay()
{
	Super::BeginPlay();
	
}
