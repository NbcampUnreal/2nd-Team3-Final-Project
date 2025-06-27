#include "GhostActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"

UGhostActorComponent::UGhostActorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UGhostActorComponent::BeginPlay()
{
    Super::BeginPlay();

    // 0.01초 간격으로 부드럽게 이동
    GetWorld()->GetTimerManager().SetTimer(GhostFollowTimerHandle, this, &UGhostActorComponent::GhostFollowPlayer, 0.01f, true);
}

void UGhostActorComponent::GhostFollowPlayer()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn || !GetOwner()) return;

    AActor* Owner = GetOwner();
    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    FVector OwnerLocation = Owner->GetActorLocation();

    float Distance = FVector::Dist(PlayerLocation, OwnerLocation);

    // 너무 멀면 순간이동 + 무조건 따라가기 시작
    if (Distance > TeleportThreshold)
    {
        FVector TeleportLocation = PlayerLocation + PlayerPawn->GetActorForwardVector() * -100.f;
        Owner->SetActorLocation(TeleportLocation);
        UE_LOG(LogTemp, Warning, TEXT("Ghost NPC teleported behind player."));
        bIsFollowing = true; // 강제로 추적 상태로 복귀
        return;
    }

    //  도착하면 추적 멈춤
    if (bIsFollowing)
    {
        if (Distance > StopThreshold)
        {
            // 이동
            FVector Direction = (PlayerLocation - OwnerLocation).GetSafeNormal();
            FVector NewLocation = OwnerLocation + Direction * Speed * GetWorld()->GetDeltaSeconds();
            Owner->SetActorLocation(NewLocation);
        }
        else
        {
            // 도착 → 추적 멈춤 전환
            Owner->SetActorLocation(PlayerLocation); // 완벽 고정
            bIsFollowing = false;
        }
    }
    else
    {
        //  도착 상태면 매 Tick 강제로 딱 붙여!
        Owner->SetActorLocation(PlayerLocation);

        // 플레이어가 다시 멀어지면 추적 상태로 복귀
        if (Distance > StopThreshold)
        {
            bIsFollowing = true;
        }
    }

    // 항상 바라보기
    FVector ToPlayer = PlayerLocation - OwnerLocation;
    FRotator LookAtRotation = ToPlayer.Rotation();
    Owner->SetActorRotation(LookAtRotation);
}