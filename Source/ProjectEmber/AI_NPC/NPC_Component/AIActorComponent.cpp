#include "AIActorComponent.h"
#include "AI_NPC/AINPCController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Engine/World.h"

UAIActorComponent::UAIActorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;  // 컴포넌트용
}

void UAIActorComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        GEngine->Exec(GetWorld(), TEXT("ShowNavigation"));
    }

    GetWorld()->GetTimerManager().SetTimer(DistanceCheckTimerHandle, this, &UAIActorComponent::CheckPlayerDistance, 1.0f, true);
    GetWorld()->GetTimerManager().SetTimer(FacePlayerTimerHandle, this, &UAIActorComponent::FacePlayer, 0.02f, true);
}

void UAIActorComponent::CheckPlayerDistance()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn || !GetOwner()) return;

    AActor* Owner = GetOwner();
    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), Owner->GetActorLocation());

    const float TeleportThreshold = 2000.f;
    const float FollowThreshold = 10.f;

    if (Distance > TeleportThreshold)
    {
        FVector TeleportLocation = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector() * -100.f;
        Owner->SetActorLocation(TeleportLocation);
        UE_LOG(LogTemp, Warning, TEXT("AINPC teleported behind player."));
        return;
    }

    // AIController는 Pawn을 통해 접근
    APawn* OwnerPawn = Cast<APawn>(Owner);
    if (OwnerPawn)
    {
        if (AAINPCController* AIController = Cast<AAINPCController>(OwnerPawn->GetController()))
        {
            if (Distance > FollowThreshold)
            {
                AIController->GetBlackboardComponent()->SetValueAsObject("Target", PlayerPawn);
            }
            else
            {
                AIController->GetBlackboardComponent()->ClearValue("Target");
            }
        }
    }
}

void UAIActorComponent::FacePlayer()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn || !GetOwner()) return;

    AActor* Owner = GetOwner();
    FVector ToPlayer = (PlayerPawn->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
    FRotator TargetRot = ToPlayer.Rotation();
    TargetRot.Pitch = 0.f;
    TargetRot.Roll = 0.f;

    Owner->SetActorRotation(FMath::RInterpTo(Owner->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 5.0f));
}