#include "QuestGiverComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

UQuestGiverComponent::UQuestGiverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    ExclamationMarkComponent = nullptr;
    ExclamationMarkMesh = nullptr;

    QuestionMarkComponent = nullptr;
    QuestionMarkMesh = nullptr;
}

void UQuestGiverComponent::BeginPlay()
{
    Super::BeginPlay();

    ShowExclamationMark();
}

USceneComponent* UQuestGiverComponent::GetAttachTarget() const
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor) return nullptr;

    // 기본 RootComponent
    USceneComponent* AttachTarget = OwnerActor->GetRootComponent();

    // HeadMarker 찾기
    bool bFoundHeadMarker = false;
    TArray<USceneComponent*> Components;
    OwnerActor->GetComponents<USceneComponent>(Components);

    for (USceneComponent* Comp : Components)
    {
        if (Comp && Comp->GetName().Equals(TEXT("HeadMarker")))
        {
            AttachTarget = Comp;
            bFoundHeadMarker = true;
            break;
        }
    }

    //  로그 출력
    if (bFoundHeadMarker)
    {
        UE_LOG(LogTemp, Warning, TEXT("[QuestGiverComponent]  Found HeadMarker! Using it as attach target."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[QuestGiverComponent] HeadMarker NOT FOUND! Using RootComponent as fallback."));
    }

    return AttachTarget;
}

void UQuestGiverComponent::InitializeExclamationMark()
{
    AActor* OwnerActor = GetOwner();
    if (!ExclamationMarkComponent && OwnerActor)
    {
        //  HeadMarker 있으면 거기에 붙인다!
        USceneComponent* AttachTarget = GetAttachTarget();
        if (!AttachTarget)
        {
            UE_LOG(LogTemp, Warning, TEXT("[QuestGiverComponent] No valid attach target!"));
            return;
        }

        ExclamationMarkComponent = NewObject<UStaticMeshComponent>(OwnerActor);
        ExclamationMarkComponent->RegisterComponent();
        ExclamationMarkComponent->AttachToComponent(AttachTarget, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

        if (ExclamationMarkMesh)
        {
            ExclamationMarkComponent->SetStaticMesh(ExclamationMarkMesh);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[QuestGiverComponent] ExclamationMarkMesh is not set!"));
        }

        ExclamationMarkComponent->SetRelativeLocation(FVector::ZeroVector);
        ExclamationMarkComponent->SetRelativeScale3D(FVector(0.5f));  // 크기 조정도 동일
        ExclamationMarkComponent->SetVisibility(false);
    }
}

void UQuestGiverComponent::InitializeQuestionMark()
{
    AActor* OwnerActor = GetOwner();
    if (!QuestionMarkComponent && OwnerActor)
    {
        USceneComponent* AttachTarget = GetAttachTarget();
        if (!AttachTarget)
        {
            UE_LOG(LogTemp, Warning, TEXT("[QuestGiverComponent] No valid attach target!"));
            return;
        }

        QuestionMarkComponent = NewObject<UStaticMeshComponent>(OwnerActor);
        QuestionMarkComponent->RegisterComponent();
        QuestionMarkComponent->AttachToComponent(AttachTarget, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

        if (QuestionMarkMesh)
        {
            QuestionMarkComponent->SetStaticMesh(QuestionMarkMesh);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[QuestGiverComponent] QuestionMarkMesh is not set!"));
        }

        QuestionMarkComponent->SetRelativeLocation(FVector::ZeroVector);
        this->QuestionMarkComponent->SetRelativeScale3D(FVector(0.5f));
        QuestionMarkComponent->SetVisibility(false);
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

    if (QuestionMarkComponent)
    {
        QuestionMarkComponent->SetVisibility(false);
    }
}

void UQuestGiverComponent::HideExclamationMark()
{
    if (ExclamationMarkComponent)
    {
        ExclamationMarkComponent->SetVisibility(false);
    }
}

void UQuestGiverComponent::ShowQuestionMark()
{
    if (!QuestionMarkComponent)
    {
        InitializeQuestionMark();
    }

    if (QuestionMarkComponent)
    {
        QuestionMarkComponent->SetVisibility(true);
    }

    if (ExclamationMarkComponent)
    {
        ExclamationMarkComponent->SetVisibility(false);
    }
}

void UQuestGiverComponent::HideQuestionMark()
{
    if (QuestionMarkComponent)
    {
        QuestionMarkComponent->SetVisibility(false);
    }
}

void UQuestGiverComponent::HideAllMarks()
{
    if (ExclamationMarkComponent)
    {
        ExclamationMarkComponent->SetVisibility(false);
    }
    if (QuestionMarkComponent)
    {
        QuestionMarkComponent->SetVisibility(false);
    }
}
