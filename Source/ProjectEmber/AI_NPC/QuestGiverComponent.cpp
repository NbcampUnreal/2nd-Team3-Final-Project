#include "QuestGiverComponent.h"
#include "QuestReceiverComponent.h"
#include "AIController.h"
#include "Blueprint/UserWidget.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

UQuestGiverComponent::UQuestGiverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    ExclamationMarkComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExclamationMarkComponent"));
    QuestionMarkComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("QuestionMarkComponent"));
}

void UQuestGiverComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner || !Owner->GetRootComponent()) return;

    auto CreateMarker = [&](UStaticMesh* Mesh, UStaticMeshComponent*& OutComponent)
        {
            if (!Mesh) return;

            OutComponent = NewObject<UStaticMeshComponent>(Owner);
            OutComponent->RegisterComponent();
            OutComponent->SetStaticMesh(Mesh);
            OutComponent->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            OutComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
            OutComponent->SetVisibility(false);

        
            OutComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            OutComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        };

    CreateMarker(ExclamationMarkMesh, ExclamationMarkComponent);
    CreateMarker(QuestionMarkMesh, QuestionMarkComponent);

    SetupDebugBindings();
    UpdateQuestMarker();
}

void UQuestGiverComponent::SetupComponentDispatchers(AActor* NPCRef, UQuestReceiverComponent* QuestReceiver)
{
    if (!QuestReceiver) return;

    CachedReceiver = QuestReceiver;

    QuestReceiver->OnQuestAccepted.AddDynamic(this, &UQuestGiverComponent::OnQuestAccepted);
    QuestReceiver->OnQuestAbandoned.AddDynamic(this, &UQuestGiverComponent::OnQuestAbandoned);
    QuestReceiver->OnQuestCompleted.AddDynamic(this, &UQuestGiverComponent::OnQuestCompleted);
    QuestReceiver->OnQuestUpdated.AddDynamic(this, &UQuestGiverComponent::OnQuestUpdated);

    UpdateQuestMarker();
}

void UQuestGiverComponent::UpdateQuestMarker()
{
    if (!CachedReceiver) return;

    if (ExclamationMarkComponent) ExclamationMarkComponent->SetVisibility(false);
    if (QuestionMarkComponent) QuestionMarkComponent->SetVisibility(false);

    int32 IncompleteCount = 0;
    int32 CompleteCount = 0;

    for (const FQuestStorageInfo& Quest : CachedReceiver->GetQuestLog())
    {
        if (Quest.bIsComplete) CompleteCount++;
        else IncompleteCount++;
    }

    if (CompleteCount > 0 && QuestionMarkComponent)
    {
        QuestionMarkComponent->SetVisibility(true);
    }
    else if (IncompleteCount == 0 && ExclamationMarkComponent)
    {
        ExclamationMarkComponent->SetVisibility(true);
    }
}

void UQuestGiverComponent::SetupDebugBindings()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC && PC->InputComponent)
    {
        FInputActionBinding& Bind = PC->InputComponent->BindAction("DebugQuestMarker", IE_Pressed, this, &UQuestGiverComponent::UpdateQuestMarker);
        Bind.bConsumeInput = true;
    }
}

void UQuestGiverComponent::OnQuestAccepted(const FQuestStorageInfo& QuestInfo)
{
    UE_LOG(LogTemp, Log, TEXT("Quest Accepted: %s"), *QuestInfo.QuestName);
    UpdateQuestMarker();
}

void UQuestGiverComponent::OnQuestAbandoned(const FQuestStorageInfo& QuestInfo)
{
    UE_LOG(LogTemp, Warning, TEXT("Quest Abandoned: %s"), *QuestInfo.QuestName);
    UpdateQuestMarker();
}

void UQuestGiverComponent::OnQuestCompleted(const FQuestStorageInfo& QuestInfo)
{
    UE_LOG(LogTemp, Log, TEXT("Quest Completed: %s"), *QuestInfo.QuestName);
    UpdateQuestMarker();
}

void UQuestGiverComponent::OnQuestUpdated(const FQuestStorageInfo& QuestInfo)
{
    UE_LOG(LogTemp, Log, TEXT("Quest Updated: %s"), *QuestInfo.QuestName);
    UpdateQuestMarker();
}
