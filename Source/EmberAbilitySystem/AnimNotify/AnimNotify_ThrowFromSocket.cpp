// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ThrowFromSocket.h"

#include "EmberLog/EmberLog.h"
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

	// 소켓 위치
	const FVector SocketLocation = VisualMesh->GetSocketLocation(SocketName);

	// 발사 방향 회전 계산
	const FVector Forward = Owner->GetActorForwardVector();
	const FRotator SpawnRotation = Forward.Rotation();

	// 위치를 약간 앞쪽으로 오프셋
	const FVector SpawnLocation = SocketLocation + Forward * 20.f;

	UWorld* World = Owner->GetWorld();
	if (!World)
	{
		return;
	}
	
	// 4) 스폰
	FActorSpawnParameters Params;
	Params.Owner = Owner;
	Params.Instigator = Cast<APawn>(Owner);

	AActor* Spawned = World->SpawnActor<AActor>(ProjectileClass,SpawnLocation,SpawnRotation,Params);
	if (!Spawned)
	{
		return;
	}
	
	// 5) Velocity 세팅
	if (UProjectileMovementComponent* ProjMov = Spawned->FindComponentByClass<UProjectileMovementComponent>())
	{
		Spawned->SetActorRotation(Forward.Rotation());
		ProjMov->Velocity = Forward * ThrowSpeed;
	}
	else if (UStaticMeshComponent* SM = Spawned->FindComponentByClass<UStaticMeshComponent>())
	{
		if (SM->IsSimulatingPhysics())
		{
			SM->AddImpulse(Forward * ThrowSpeed, NAME_None, true);
		}
	}
}
