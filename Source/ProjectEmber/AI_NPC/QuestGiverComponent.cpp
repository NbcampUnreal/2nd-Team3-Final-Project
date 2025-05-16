#include "QuestGiverComponent.h"
#include "QuestReceiverComponent.h"
#include "AIController.h"
#include "Blueprint/UserWidget.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"

UQuestGiverComponent::UQuestGiverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UQuestGiverComponent::BeginPlay()
{
    Super::BeginPlay();

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn)
    {
        CachedAIController = Cast<AAIController>(OwnerPawn->GetController());

        if (QuestWidgetClass)
        {
            QuestWidget = CreateWidget<UUserWidget>(GetWorld(), QuestWidgetClass);
            if (QuestWidget)
            {
                QuestWidget->AddToViewport();
                QuestWidget->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }

    SetupDebugBindings();
    UpdateQuestMarker(); // 마커 초기화용 호출 추가
}

void UQuestGiverComponent::SetupComponentDispatchers(AActor* NPCRef, UQuestReceiverComponent* QuestReceiver)
{
    if (!QuestReceiver) return;

    CachedReceiver = QuestReceiver;

    QuestReceiver->OnQuestAccepted.AddDynamic(this, &UQuestGiverComponent::OnQuestAccepted);
    QuestReceiver->OnQuestAbandoned.AddDynamic(this, &UQuestGiverComponent::OnQuestAbandoned);
    QuestReceiver->OnQuestCompleted.AddDynamic(this, &UQuestGiverComponent::OnQuestCompleted);
    QuestReceiver->OnQuestUpdated.AddDynamic(this, &UQuestGiverComponent::OnQuestUpdated);

    UpdateQuestMarker(); // 연결 후 마커 상태 즉시 갱신
}

void UQuestGiverComponent::UpdateQuestMarker()
{
    if (!CachedReceiver)
    {
        if (ExclamationMarkSM) ExclamationMarkSM->SetVisibility(false);
        if (QuestionMarkSM) QuestionMarkSM->SetVisibility(true);
        return;
    }

    int32 ActiveQuests = 0;
    for (const FQuestStorageInfo& Quest : CachedReceiver->GetQuestLog())
    {
        if (!Quest.bIsComplete)
        {
            ActiveQuests++;
        }
    }

    if (ExclamationMarkSM)
    {
        ExclamationMarkSM->SetVisibility(ActiveQuests > 0);
    }

    if (QuestionMarkSM)
    {
        QuestionMarkSM->SetVisibility(ActiveQuests == 0);
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
