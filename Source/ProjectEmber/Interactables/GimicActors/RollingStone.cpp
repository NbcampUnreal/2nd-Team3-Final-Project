#include "Interactables/GimicActors/RollingStone.h"
#include "Components/SphereComponent.h"

ARollingStone::ARollingStone()
{
 	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MeshComp->SetMobility(EComponentMobility::Movable);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(true);
	MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	
	DeathTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("DestroyerBox"));
	DeathTrigger->SetupAttachment(MeshComp);
	DeathTrigger->InitSphereRadius(100.0f);
	DeathTrigger->SetCollisionProfileName(TEXT("Trigger"));

	DeathTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARollingStone::OnOverlapBegin);
	DeathTrigger->OnComponentEndOverlap.AddDynamic(this, &ARollingStone::OnOverlapEnd);
}

void ARollingStone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ARollingStone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ARollingStone::BeginPlay()
{
	Super::BeginPlay();
	
}

