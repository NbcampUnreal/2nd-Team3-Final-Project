#include "Interactables/BasePickupActor.h"

#include "Components/BoxComponent.h"

ABasePickupActor::ABasePickupActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// 이동 가능하게 설정
	MeshComp->SetMobility(EComponentMobility::Movable);

	// 물리 시뮬레이션 활성화
	MeshComp->SetSimulatePhysics(true);

	// 중력 사용 여부 (기본값 true)
	MeshComp->SetEnableGravity(true);

	// 충돌 프리셋: 물리 바디용으로 설정
	MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void ABasePickupActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePickupActor::Interact_Implementation(AActor* Caller)
{
	Destroy();
}
