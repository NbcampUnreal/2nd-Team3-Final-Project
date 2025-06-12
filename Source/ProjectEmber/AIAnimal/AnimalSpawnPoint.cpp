

#include "AnimalSpawnPoint.h"

#include "AnimalSpawner.h"
#include "BaseAIAnimal.h"
#include "Components/BoxComponent.h"


AAnimalSpawnPoint::AAnimalSpawnPoint()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
}

void AAnimalSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAnimalSpawnPoint::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AAnimalSpawnPoint::OnEndOverlap);
}

void AAnimalSpawnPoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsInHere = true; 
}

void AAnimalSpawnPoint::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsInHere = false; 
}

void AAnimalSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAnimalSpawnPoint::GetIsInHere() const
{
	return bIsInHere;
}

int32 AAnimalSpawnPoint::GetAliveAnimalsInBox() const
{
	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors, ABaseAIAnimal::StaticClass());
	return OverlappingActors.Num();
}

bool AAnimalSpawnPoint::GetIsCreated() const
{
	return bIsCreated;
}

void AAnimalSpawnPoint::SetIsCreated(bool Inbool)
{
	bIsCreated = Inbool;
}
