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
#include "PlayerQuestWidget.h"
#include "Components/TextBlock.h"
#include "DialogueDataRow.h"
#include "EnhancedInputSubsystems.h"
#include "UI/HUD/EmberMainHUD.h"
#include "Character/EmberComponents/InteractionComponent.h"
#include "EngineUtils.h"
#include "Quest/QuestSubsystem.h"
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
//트리거 설정 위젯 설정
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
        // 상호작용 위젯
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
//대화 위젯관련
void UDialogueComponent::LoadDialogueFromDataTable(bool bResetDialogueIndex, FName InObjectiveTag)
{
    LinesOfDialogue.Empty();

    if (!QuestAsset) return;

    UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    if (!QuestSubsystem) return;

    const int32 StepIndex = QuestSubsystem->GetCurrentStepIndexForQuest(QuestAsset->QuestID, false);

    const TArray<FQuestStep>& Steps = QuestAsset->Steps;
    AActor* Owner = GetOwner();

    // ① 퀘스트 아직 수락 전 (StepIndex == INDEX_NONE)
    if (StepIndex == INDEX_NONE)
    {
        if (Steps.Num() > 0)
        {
            const FQuestStep& FirstStep = Steps[0];
            if (Owner == FirstStep.QuestGiver.Get())
            {
                LinesOfDialogue = FirstStep.GiverDialogueLines;
            }
        }
    }
    else if (Steps.IsValidIndex(StepIndex))
    {
        const FQuestStep& CurrentStep = Steps[StepIndex];

        if (Owner == CurrentStep.CompletionGiver.Get())
        {
            LinesOfDialogue = CurrentStep.CompleteDialogueLines;
        }
        else if (Owner == CurrentStep.QuestGiver.Get())
        {
            LinesOfDialogue = CurrentStep.GiverDialogueLines;
        }
    }

    if (bResetDialogueIndex)
    {
        CurrentDialogueIndex = 0;
    }
}
void UDialogueComponent::SetInputMappingContexts(TArray<UInputMappingContext*> MappingContexts, bool bClearExisting)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
    if (!LocalPlayer) return;

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
    {
        if (bClearExisting)
        {
            Subsystem->ClearAllMappings();
        }

        for (int32 i = 0; i < MappingContexts.Num(); ++i)
        {
            if (MappingContexts[i])
            {
                Subsystem->AddMappingContext(MappingContexts[i], i); 
            }
        }
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
    if (!bPlayerInRange || !DialogueWidgetClass || bDialogueFinished) return;

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
        const int32 HighZOrder = 999;
        DialogueWidget->AddToViewport(HighZOrder);

        //게임 입력 + UI 입력을 동시에 허용하는 모드
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(DialogueWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;

        
        SetInputMappingContexts({ UIInputMappingContext }, true);

        if (TalkPromptWidget)
        {
            TalkPromptWidget->SetVisibility(false);
        }

        UE_LOG(LogTemp, Warning, TEXT("[Interact] DialogueWidget created and shown (ZOrder %d)."), HighZOrder);
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
            // FText 그대로 출력
            ChatBox->SetText(LinesOfDialogue[CurrentDialogueIndex]);
            UE_LOG(LogTemp, Warning, TEXT("[AdvanceDialogue] Line %d shown."), CurrentDialogueIndex + 1);
            ++CurrentDialogueIndex;
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
        if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
        {
            FGameplayEventData EventData;
            EventData.EventTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.Dialogue.DialogueReceiver"));
            EventData.Instigator = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
            EventData.Target = GetOwner();
            //진행 조건 충족 여부
            QuestSubsystem->OnGameEvent(EventData.EventTag, EventData);

            if (QuestAsset && QuestSubsystem->IsQuestCompleted(QuestAsset->QuestID))
            {
                ShowQuestCompleteWidget(QuestAsset);
                return;
            }
        }
        FTimerHandle DummyHandle;
        GetWorld()->GetTimerManager().SetTimer(DummyHandle, [this]()
            {
                ShowQuestUI();
            }, 0.01f, false);
    }
}

void UDialogueComponent::InitializeAndDisplayWidget(UUserWidget* Widget)
{
    if (!Widget) return;
    Widget->AddToViewport(999);

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    PC->bShowMouseCursor = true;
    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(Widget->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputMode);

    SetDialogueVisualState(true);
}
void UDialogueComponent::SetDialogueVisualState(bool bShowUI)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PC || !Player) return;

    PC->bShowMouseCursor = bShowUI;

    if (bShowUI)
    {
        if (Player->GetMesh()) Player->GetMesh()->SetVisibility(false, true);
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
    }
    else
    {
        if (Player->GetMesh())
            Player->GetMesh()->SetVisibility(true, true);

        PC->SetInputMode(FInputModeGameOnly());
        PC->SetViewTargetWithBlend(PC->GetPawn(), 0.5f);
        SetInputMappingContexts({ GameplayInputMappingContext, GameplayUIInputMappingContext }, true);
        PC->bShowMouseCursor = false;
        if (DialogueWidget)
        {
            DialogueWidget->RemoveFromParent();
            DialogueWidget = nullptr;
        }
    }
}

void UDialogueComponent::ShowQuestUI()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC || !QuestAsset) return;

    if (DialogueWidget)
    {
        DialogueWidget->RemoveFromParent();
        DialogueWidget = nullptr;
    }

    UQuestWidget* QuestUI = CreateWidget<UQuestWidget>(PC, QuestWidgetClass);
    if (!QuestUI) return;

    bool bIsComplete = false;
    bool bIsAccepted = false;

    if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
        bIsComplete = QuestSubsystem->IsQuestCompleted(QuestAsset->QuestID);
        bIsAccepted = QuestSubsystem->IsQuestAccepted(QuestAsset->QuestID);
    }

    QuestUI->SetQuestInfoFromDataAsset(QuestAsset, bIsComplete, bIsAccepted);

    QuestUI->OnQuestAccepted.AddLambda([this, QuestUI]() {
        if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
        {
            Subsystem->TryStartQuest(QuestAsset->QuestID, true);
            UE_LOG(LogTemp, Warning, TEXT(">>> QuestAsset valid: %s"), *QuestAsset->QuestID.ToString());
        }
        QuestUI->RemoveFromParent();
        SetDialogueVisualState(false);
        });

    QuestUI->OnQuestRefused.AddLambda([this, QuestUI]() {
        QuestUI->RemoveFromParent();
        SetDialogueVisualState(false);
        bDialogueFinished = false;
        });

    InitializeAndDisplayWidget(QuestUI);
}

void UDialogueComponent::ShowQuestCompleteWidget(const UQuestDataAsset* InQuestAsset)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC || !QuestCompleteWidgetClass || !QuestAsset) return;

    UQuestWidget* CompleteWidget = CreateWidget<UQuestWidget>(PC, QuestCompleteWidgetClass);
    if (!CompleteWidget) return;

    if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
        const bool bIsComplete = QuestSubsystem->IsQuestCompleted(QuestAsset->QuestID);
        const bool bIsAccepted = QuestSubsystem->IsQuestAccepted(QuestAsset->QuestID);

        CompleteWidget->SetQuestInfoFromDataAsset(QuestAsset, bIsComplete, bIsAccepted);
    }

    CompleteWidget->OnQuestCompleted.AddLambda([this, PC, CapturedQuestAsset = QuestAsset, CompleteWidget]() {
        if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
        {
            Subsystem->CompleteQuest(CapturedQuestAsset->QuestID);
        }

        if (CompleteWidget && CompleteWidget->IsInViewport())
        {
            CompleteWidget->RemoveFromParent();
        }

        SetDialogueVisualState(false);
        PC->SetInputMode(FInputModeGameOnly());
        PC->SetViewTargetWithBlend(PC->GetPawn(), 0.5f);
        });

    if (DialogueWidget)
    {
        DialogueWidget->RemoveFromParent();
        DialogueWidget = nullptr;
    }

    InitializeAndDisplayWidget(CompleteWidget);
}


void UDialogueComponent::RepositionNPCForDialogue()
{
    AActor* NPC = GetOwner();
    if (!NPC) return;

    UAttatchAIDialogueCamera* DialogueCam = NPC->FindComponentByClass<UAttatchAIDialogueCamera>();
    if (!DialogueCam) return;

    FVector DirectionToCamera = DialogueCam->GetComponentLocation() - NPC->GetActorLocation();
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
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            PC->SetViewTargetWithBlend(TempCam, 0.25f, EViewTargetBlendFunction::VTBlend_EaseInOut);
        }
    }
}

void UDialogueComponent::UpdateQuestLogWidgetFromAsset(const UQuestDataAsset* InQuestAsset)
{
    if (!InQuestAsset) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    if (AEmberMainHUD* HUD = Cast<AEmberMainHUD>(PC->GetHUD()))
    {
        if (UPlayerQuestWidget* QuestLogWidget = HUD->GetQuestLogWidget())
        {
            bool bIsComplete = false;
            bool bIsAccepted = false;

            if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
            {
                bIsComplete = QuestSubsystem->IsQuestCompleted(InQuestAsset->QuestID);
                bIsAccepted = QuestSubsystem->IsQuestAccepted(InQuestAsset->QuestID);
            }

            QuestLogWidget->SetQuestInfoFromDataAsset(InQuestAsset, bIsComplete, bIsAccepted);
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