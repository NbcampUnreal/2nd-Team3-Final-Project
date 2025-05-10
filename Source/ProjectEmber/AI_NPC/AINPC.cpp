#include "AINPC.h"
#include "AINPCController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AAINPC::AAINPC()
{
    PrimaryActorTick.bCanEverTick = false;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAINPCController::StaticClass();
}

void AAINPC::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        GEngine->Exec(GetWorld(), TEXT("ShowNavigation"));
    }

    // 거리 체크 및 추적
    GetWorldTimerManager().SetTimer(DistanceCheckTimerHandle, this, &AAINPC::CheckPlayerDistance, 1.0f, true);

    // 항상 플레이어를 바라보게 회전
    GetWorldTimerManager().SetTimer(FacePlayerTimerHandle, this, &AAINPC::FacePlayer, 0.02f, true);
}

void AAINPC::CheckPlayerDistance()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;


    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
    const float TeleportThreshold = 2000.f;
    const float FollowThreshold = 10.f;

    if (Distance > TeleportThreshold)
    {
        // 플레이어 뒤쪽 300 유닛 위치로 순간이동
        FVector TeleportLocation = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector() * -10.f;
        SetActorLocation(TeleportLocation);

        UE_LOG(LogTemp, Warning, TEXT("AINPC teleported behind player."));
        return;
    }

    // AI에게 타겟 설정 (추적 시작)
    if (AAINPCController* AIController = Cast<AAINPCController>(GetController()))
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

void AAINPC::FacePlayer()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    FVector ToPlayer = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FRotator TargetRot = ToPlayer.Rotation();
    TargetRot.Pitch = 0.f;
    TargetRot.Roll = 0.f;

    SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 5.0f));
}