// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ThrowFromSocket.h"

#include "AlsCharacter.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UAnimNotify_ThrowFromSocket::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !ProjectileClass)
	{
		return;
	}
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}
	
	// 실제 비주얼 메쉬 찾기
	const USkeletalMeshComponent* VisualMesh = nullptr;
	TArray<USkeletalMeshComponent*> MeshComps;
	Owner->GetComponents(MeshComps);
	for (USkeletalMeshComponent* SkeletalMeshComponent : MeshComps)
	{
		if (SkeletalMeshComponent->GetName() == TEXT("SK_LittleBoyRyan"))
		{
			VisualMesh = SkeletalMeshComponent;
			break;
		}
	}
	if (!VisualMesh)
	{
		EMBER_LOG(LogEmber, Warning, TEXT("ThrowNotify: SK_LittleBoyRyan component not found."));
		return;
	}
	
	/*const FVector SocketLocation = VisualMesh->GetSocketLocation(SocketName);
	UWorld* World = Owner->GetWorld();
	APlayerController* PC = Cast<APlayerController>(Cast<ACharacter>(Owner)->GetController());
	FVector ViewLoc;
	FRotator ViewRot;
	if (PC && Cast<AAlsCharacter>(Owner)->GetRotationMode() == AlsRotationModeTags::Aiming)
	{*/
	// 1) 소켓(어깨) 위치
	const FVector SocketLocation = VisualMesh->GetSocketLocation(SocketName);

	// 2) 카메라 위치·회전 (뷰포인트) 얻기
	APlayerController* PC = Cast<APlayerController>(Cast<ACharacter>(Owner)->GetController());
	FVector CameraLoc;
	FRotator CameraRot;
	if (PC && Cast<AAlsCharacter>(Owner)->GetRotationMode() == AlsRotationModeTags::Aiming)
	{
		PC->GetPlayerViewPoint(CameraLoc, CameraRot);
	}
	else
	{
		// 폴백
		CameraLoc  = SocketLocation;
		CameraRot  = Owner->GetActorRotation();
	}

	// 3) 화면 중앙 방향 벡터
	FVector CamForward = CameraRot.Vector();

	// 4) 라인 트레이스: 카메라에서 화면 중앙 방향으로 쏴서 충돌점 찾기
	UWorld* World = Owner->GetWorld();
	if (!World) return;

	FHitResult Hit;
	FVector TraceStart = CameraLoc;
	FVector TraceEnd   = TraceStart + CamForward * 10000.f;  // 충분히 먼 거리
	FCollisionQueryParams Params(NAME_None, /*bTraceComplex=*/true, Owner);
	Params.bReturnPhysicalMaterial = false;

	FVector TargetPoint;
	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		TargetPoint = Hit.Location;
	}
	else
	{
		TargetPoint = TraceEnd;
	}

	// 5) 진짜 발사 방향: 소켓 위치 → 타겟 포인트
	FVector ShootDir = (TargetPoint - SocketLocation).GetSafeNormal();

	// 6) 회전·위치 재계산
	FRotator SpawnRot      = ShootDir.Rotation();
	FVector   SpawnLoc      = SocketLocation + ShootDir * 20.f;

	// 7) 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner     = Owner;
	SpawnParams.Instigator= Cast<APawn>(Owner);

	AActor* Projectile = World->SpawnActor<AActor>(
		ProjectileClass,
		SpawnLoc,
		SpawnRot,
		SpawnParams
	);
	if (!Projectile) return;

	// 8) 초기 속도 세팅
	if (auto* ProjMov = Projectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		ProjMov->Velocity = ShootDir * ThrowSpeed;
	}
	else if (auto* SM = Projectile->FindComponentByClass<UStaticMeshComponent>())
	{
		if (SM->IsSimulatingPhysics())
		{
			SM->AddImpulse(ShootDir * ThrowSpeed, NAME_None, true);
		}
	}
}
