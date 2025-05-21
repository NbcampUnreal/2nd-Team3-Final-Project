#include "DialogueComponent.h"
#include "NPCSphereComponent.h"
#include "NPCTalkWidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Camera/CameraActor.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PrimitiveComponent.h"
#include "Interactables/Interactable.h"
#include "AttatchAIDialogueCamera.h"
#include "QuestReceiverComponent.h"
#include "QuestDataRow.h"
#include "Components/TextBlock.h"
#include "DialogueDataRow.h"
#include "EnhancedInputSubsystems.h"
#include "Character/EmberComponents/InteractionComponent.h"
#include "EngineUtils.h"
#include "QuestWidget.h"

UDialogueComponent::UDialogueComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bPlayerInRange = false;
    DialogueCameraActor = nullptr;
    DialogueWidget = nullptr;
    InteractionTrigger = nullptr;
    TalkPromptWidget = nullptr;
    CurrentDialogueIndex = 0;
    bDialogueFinished = false;
}
void UDialogueComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner) return;

    InteractionTrigger = Owner->FindComponentByClass<UNPCSphereComponent>();
    TalkPromptWidget = Owner->FindComponentByClass<UNPCTalkWidgetComponent>();

    if (TalkPromptWidget && TalkPromptWidgetClass)
    {
        TalkPromptWidget->SetWidgetClass(TalkPromptWidgetClass);
        TalkPromptWidget->SetVisibility(false);

        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
            if (TalkPromptWidget)
            {
                if (UUserWidget* UI = TalkPromptWidget->GetUserWidgetObject())
                {
                    UI->SetVisibility(ESlateVisibility::Hidden);
                }
            }
            }, 0.01f, false);
    }

    if (InteractionTrigger)
    {
        InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &UDialogueComponent::OnPlayerEnterRadius);
        InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &UDialogueComponent::OnPlayerExitRadius);
    }

    LoadDialogueFromDataTable(true);
}

void UDialogueComponent::LoadDialogueFromDataTable(bool bResetDialogueIndex)
{
    if (!DialogueDataTable || DialogueRowName.IsNone()) return;

    const FDialogueDataRow* Row = DialogueDataTable->FindRow<FDialogueDataRow>(DialogueRowName, TEXT("LoadDialogue"));
    if (Row)
    {
        LinesOfDialogue = Row->DialogueLines;
        if (bResetDialogueIndex)
        {
            CurrentDialogueIndex = 0;
        }
        UE_LOG(LogTemp, Warning, TEXT("Dialogue Loaded. Total Lines: %d"), LinesOfDialogue.Num());
    }
}
void UDialogueComponent::SetInputMappingContext(UInputMappingContext* MappingContext, bool bClearExisting)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC || !MappingContext) return;

    ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
    if (!LocalPlayer) return;

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
    {
        if (bClearExisting)
        {
            Subsystem->ClearAllMappings();
        }
        Subsystem->AddMappingContext(MappingContext, 0);

        
    }
}

void UDialogueComponent::OnPlayerEnterRadius(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA<ACharacter>() && TalkPromptWidget)
    {
        TalkPromptWidget->SetVisibility(true);
        if (UUserWidget* UI = TalkPromptWidget->GetUserWidgetObject())
        {
            UI->SetVisibility(ESlateVisibility::Visible);
        }
        bPlayerInRange = true;
    }
}

void UDialogueComponent::OnPlayerExitRadius(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (Cast<ACharacter>(OtherActor) && TalkPromptWidget)
    {
        TalkPromptWidget->SetVisibility(false);
        if (UUserWidget* UI = TalkPromptWidget->GetUserWidgetObject())
        {
            UI->SetVisibility(ESlateVisibility::Hidden);
        }

        bPlayerInRange = false;
        ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (Player && Player->GetMesh())
        {
            Player->GetMesh()->SetVisibility(true, true);
        }
        if (DialogueWidget)
        {
            DialogueWidget->RemoveFromParent();
            DialogueWidget = nullptr;

            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC)
            {
                PC->SetViewTargetWithBlend(PC->GetPawn(), 0.5f);
                PC->bShowMouseCursor = false;
                PC->SetInputMode(FInputModeGameOnly());
            }
        }
    }
}

void UDialogueComponent::Interact()
{
    if (!bPlayerInRange || !DialogueWidgetClass) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AActor* OwnerActor = GetOwner();

    if (!PC || !OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueComponent::Interact] Missing PC or Owner"));
        return;
    }
    // 대화 상태 초기화
    bDialogueFinished = false;
    CurrentDialogueIndex = 0;
    LoadDialogueFromDataTable(true);

    RepositionNPCForDialogue();
    PositionDetachedCamera();

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (Player && Player->GetMesh())
    {
        Player->GetMesh()->SetVisibility(false, true);
    }

    DialogueWidget = CreateWidget<UUserWidget>(PC, DialogueWidgetClass);
    if (DialogueWidget)
    {
        DialogueWidget->AddToViewport();

        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(DialogueWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);

        SetInputMappingContext(UIInputMappingContext);

        if (TalkPromptWidget)
        {
            TalkPromptWidget->SetVisibility(false);
        }

        UE_LOG(LogTemp, Warning, TEXT("[Interact] DialogueWidget created and shown."));
    }

    AdvanceDialogue();
}

void UDialogueComponent::AdvanceDialogue()
{
    if (bDialogueFinished)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AdvanceDialogue] Dialogue already finished, skipping."));
        return;
    }

    if (!DialogueWidget || LinesOfDialogue.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AdvanceDialogue] No dialogue to advance."));
        return;
    }
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PC->bShowMouseCursor = false; 
    }
    if (CurrentDialogueIndex < LinesOfDialogue.Num())
    {
        if (UTextBlock* ChatBox = Cast<UTextBlock>(DialogueWidget->GetWidgetFromName("NPC_ChatBox")))
        {
            ChatBox->SetText(FText::FromString(LinesOfDialogue[CurrentDialogueIndex++]));
            UE_LOG(LogTemp, Warning, TEXT("[AdvanceDialogue] Line %d shown."), CurrentDialogueIndex);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[AdvanceDialogue] NPC_ChatBox not found."));
        }
    }
    else
    {
        bDialogueFinished = true;

        if (AActor* Owner = GetOwner())
        {
            if (UInteractionComponent* Interact = Owner->FindComponentByClass<UInteractionComponent>())
            {
                Interact->SetCurrentInteractable(nullptr);
                UE_LOG(LogTemp, Warning, TEXT("[AdvanceDialogue] Interactable cleared after full dialogue."));
            }
        }
        FTimerHandle DummyHandle;
        GetWorld()->GetTimerManager().SetTimer(DummyHandle, [this]()
            {
                ShowQuestUI(); 
            }, 0.01f, false);
    }
}

void UDialogueComponent::ShowQuestUI()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!Player) return;

    if (DialogueWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShowQuestUI] Removing lingering DialogueWidget."));
        DialogueWidget->RemoveFromParent();
        DialogueWidget = nullptr;
    }

    UQuestReceiverComponent* QuestReceiver = Player->FindComponentByClass<UQuestReceiverComponent>();
    if (!QuestReceiver)
    {
        UE_LOG(LogTemp, Error, TEXT("[ShowQuestUI] QuestReceiverComponent not found on player."));
        return;
    }

    if (!QuestWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[ShowQuestUI] QuestWidgetClass is NULL."));
        return;
    }

    UQuestWidget* QuestUI = CreateWidget<UQuestWidget>(PC, QuestWidgetClass);
    if (!QuestUI)
    {
        UE_LOG(LogTemp, Error, TEXT("[ShowQuestUI] QuestUI is NULL - CreateWidget failed."));
        return;
    }

    QuestUI->AddToViewport();
    PC->bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(QuestUI->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputMode);

    QuestUI->QuestDataTable = QuestDataTable;
    QuestUI->QuestRowName = QuestRowName;

    QuestUI->OnQuestAccepted.BindLambda([this, PC, QuestReceiver]() {
        UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] Accept clicked."));
        QuestReceiver->AcceptQuest(this->QuestDataTable, this->QuestRowName);

        ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (Player && Player->GetMesh())
        {
            Player->GetMesh()->SetVisibility(true, true);  
        }

        SetInputMappingContext(GameplayInputMappingContext);
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
        PC->SetViewTargetWithBlend(PC->GetPawn(), 0.5f);
        });

    QuestUI->OnQuestRefused.BindLambda([this, PC, QuestUI]()
        {
            UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] Refuse clicked. Return to original state."));
            QuestUI->RemoveFromParent();

            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
            PC->SetViewTargetWithBlend(PC->GetPawn(), 0.5f);

            ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
            if (Player && Player->GetMesh())
            {
                Player->GetMesh()->SetVisibility(true, true);
            }

            SetInputMappingContext(GameplayInputMappingContext);

            if (AActor* Owner = GetOwner())
            {
                if (UInteractionComponent* Interact = Owner->FindComponentByClass<UInteractionComponent>())
                {
                    Interact->SetCurrentInteractable(Owner);
                    UE_LOG(LogTemp, Warning, TEXT("[Refuse] Interactable reset."));
                }
            }
        });
}

void UDialogueComponent::RepositionNPCForDialogue()
{
    AActor* NPC = GetOwner();
    if (!NPC) return;

    UAttatchAIDialogueCamera* DialogueCam = NPC->FindComponentByClass<UAttatchAIDialogueCamera>();
    if (!DialogueCam)
    {
        UE_LOG(LogTemp, Warning, TEXT("No AttatchAIDialogueCamera found on NPC."));
        return;
    }

    FVector NPCPosition = NPC->GetActorLocation();
    FVector CameraPosition = DialogueCam->GetComponentLocation();

    FVector DirectionToCamera = CameraPosition - NPCPosition;
    DirectionToCamera.Z = 0.0f;

    FRotator LookAtRotation = DirectionToCamera.Rotation();
    LookAtRotation.Pitch = 0.0f;
    LookAtRotation.Roll = 0.0f;

    NPC->SetActorRotation(LookAtRotation);
}

void UDialogueComponent::PositionDetachedCamera()
{
    AActor* NPC = GetOwner();
    if (!NPC) return;

    UAttatchAIDialogueCamera* CameraComp = NPC->FindComponentByClass<UAttatchAIDialogueCamera>();
    if (!CameraComp) return;

    FVector CamLoc = CameraComp->GetComponentLocation();
    FRotator CamRot = CameraComp->GetComponentRotation();

    ACameraActor* TempCam = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), CamLoc, CamRot);
    if (TempCam)
    {
        TempCam->GetCameraComponent()->SetFieldOfView(CameraComp->FieldOfView);

        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            PC->SetViewTargetWithBlend(TempCam, 0.25f, EViewTargetBlendFunction::VTBlend_EaseInOut);
        }
    }
}

bool UDialogueComponent::IsDialogueActive() const
{
    return DialogueWidget != nullptr;  
}
void UDialogueComponent::Interact_Implementation(AActor* Caller)
{
    Interact();
}

float UDialogueComponent::GetGatherTime_Implementation()
{
    return 0.0f;
}