#include "Interactables/BasePickupActor.h"

#include "Components/BoxComponent.h"

ABasePickupActor::ABasePickupActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// 이동 가능하게 설정
	MeshComp->SetMobility(EComponentMobility::Movable);

#if !UE_BUILD_SHIPPING 
	// 물리 시뮬레이션 활성화 
	MeshComp->SetSimulatePhysics(true);

	// 중력 사용 여부
	MeshComp->SetEnableGravity(true);

	// 충돌 프리셋: 물리 바디용 프로필
	MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
#else
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetEnableGravity(false);
	MeshComp->SetCollisionProfileName(TEXT("BlockAll")); 
#endif
}

void ABasePickupActor::TryInteract_Implementation(AActor* Caller)
{
	Destroy();
}

void ABasePickupActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePickupActor::Interact_Implementation(AActor* Caller)
{
	Destroy();
}
