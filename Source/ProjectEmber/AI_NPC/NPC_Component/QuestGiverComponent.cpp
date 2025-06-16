#include "QuestGiverComponent.h"
#include "GameFramework/Actor.h"

UQuestGiverComponent::UQuestGiverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // 기본값: 런타임에 직접 생성하므로 nullptr
    ExclamationMarkComponent = nullptr;
    ExclamationMarkMesh = nullptr;
}

void UQuestGiverComponent::InitializeExclamationMark()
{
    if (!ExclamationMarkComponent && GetOwner())
    {
        // 동적 생성
        ExclamationMarkComponent = NewObject<UStaticMeshComponent>(GetOwner());
        ExclamationMarkComponent->RegisterComponent();
        ExclamationMarkComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

        // 메쉬 설정
        if (ExclamationMarkMesh)
        {
            ExclamationMarkComponent->SetStaticMesh(ExclamationMarkMesh);
        }

        // 위치 조정: 머리 위로 올림 (원하는 값으로 변경 가능)
        ExclamationMarkComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

        // 처음엔 숨김
        ExclamationMarkComponent->SetVisibility(false);
    }
}

void UQuestGiverComponent::ShowExclamationMark()
{
    if (!ExclamationMarkComponent)
    {
        InitializeExclamationMark();
    }

    if (ExclamationMarkComponent)
    {
        ExclamationMarkComponent->SetVisibility(true);
    }
}

void UQuestGiverComponent::HideExclamationMark()
{
    if (ExclamationMarkComponent)
    {
        ExclamationMarkComponent->SetVisibility(false);
    }
}