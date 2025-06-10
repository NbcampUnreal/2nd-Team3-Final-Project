#include "AttatchAIDialogueCamera.h"

UAttatchAIDialogueCamera::UAttatchAIDialogueCamera()
{
    // 기본 카메라 설정 (필요 시 조정 가능)
    SetRelativeLocation(FVector(300.f, 80.f, 180.f));     // 앞 + 오른쪽 + 위
    SetRelativeRotation(FRotator(-10.f, -160.f, 0.f));    // 약간 내려다보고 플레이어 방향
    SetFieldOfView(60.f);                                            // 좁은 시야각으로 집중도 향상
}

void UAttatchAIDialogueCamera::BeginPlay()
{
    Super::BeginPlay();

    // 초기화 로그
    UE_LOG(LogTemp, Warning, TEXT("UAttatchAIDialogueCamera::BeginPlay() - Initialized"));
}