#include "QuestGiverComponent.h"
#include "GameFramework/Actor.h"

UQuestGiverComponent::UQuestGiverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // �⺻��: ��Ÿ�ӿ� ���� �����ϹǷ� nullptr
    ExclamationMarkComponent = nullptr;
    ExclamationMarkMesh = nullptr;
}

void UQuestGiverComponent::InitializeExclamationMark()
{
    if (!ExclamationMarkComponent && GetOwner())
    {
        // ���� ����
        ExclamationMarkComponent = NewObject<UStaticMeshComponent>(GetOwner());
        ExclamationMarkComponent->RegisterComponent();
        ExclamationMarkComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

        // �޽� ����
        if (ExclamationMarkMesh)
        {
            ExclamationMarkComponent->SetStaticMesh(ExclamationMarkMesh);
        }

        // ��ġ ����: �Ӹ� ���� �ø� (���ϴ� ������ ���� ����)
        ExclamationMarkComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

        // ó���� ����
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