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
#include "AI_NPC/Camera/AttatchAIDialogueCamera.h"
#include "AI_NPC/Widget/PlayerQuestWidget.h"
#include "Components/TextBlock.h"
#include "EnhancedInputSubsystems.h"
#include "UI/HUD/EmberMainHUD.h"
#include "Character/EmberComponents/InteractionComponent.h"
#include "EngineUtils.h"
#include "Quest/QuestSubsystem.h"
#include "Quest/Condition/Dialogue/DialogueQuestCondition.h"
#include "AI_NPC/Widget/QuestWidget.h"
#include "AI_NPC/Widget/QuestTracker.h"
#include "Character/EmberCharacter.h"

UDialogueComponent::UDialogueComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bPlayerInRange = false;
    DialogueWidget = nullptr;
    InteractionTrigger = nullptr;
    TalkPromptWidget = nullptr;
    CurrentDialogueIndex = 0;
    bDialogueFinished = false;
}

void UDialogueComponent::TryInteract_Implementation(AActor* Caller)
{
    Interact_Implementation(Caller);
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
    UE_LOG(LogTemp, Warning, TEXT("==== [LoadDialogueFromDataTable] 호출됨 ===="));

    if (bDialogueOverriddenByCondition)
    {
        UE_LOG(LogTemp, Warning, TEXT(" 조건 대사 우선 적용 중 → 로딩 중단"));
        return;
    }

    LinesOfDialogue.Empty();

    if (!QuestAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT(" QuestAsset 없음"));
        return;
    }

    UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    if (!QuestSubsystem)
    {
        UE_LOG(LogTemp, Warning, TEXT(" QuestSubsystem 없음"));
        return;
    }

    const int32 StepIndex = QuestSubsystem->GetCurrentStepIndexForQuest(QuestAsset->QuestID, false);
    const TArray<FQuestStep>& Steps = QuestAsset->Steps;
    AActor* Owner = GetOwner();

    UE_LOG(LogTemp, Warning, TEXT(" StepIndex: %d / Steps.Num: %d / Owner: %s"), StepIndex, Steps.Num(), *GetNameSafe(Owner));

    if (StepIndex == INDEX_NONE && Steps.Num() > 0)
    {
        const FQuestStep& FirstStep = Steps[0];
        if (Owner == FirstStep.QuestGiver.Get())
        {
            LinesOfDialogue = FirstStep.GiverDialogueLines;
            UE_LOG(LogTemp, Warning, TEXT(" 첫 스텝 QuestGiver 일치 → 대사 로딩됨"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT(" 퀘스트 미시작 상태지만 QuestGiver 아님 → 대사 없음"));
        }
    }
    else if (Steps.IsValidIndex(StepIndex))
    {
        const FQuestStep& CurrentStep = Steps[StepIndex];

        // 1) 조건 대사 우선
        for (const UQuestCondition* Condition : CurrentStep.Conditions)
        {
            const UDialogueQuestCondition* DialogueCond = Cast<UDialogueQuestCondition>(Condition);
            if (DialogueCond && DialogueCond->TargetNPCActor.IsValid())
            {
                if (DialogueCond->TargetNPCActor.Get() == Owner)
                {
                    LinesOfDialogue = DialogueCond->DialogueLines;
                    UE_LOG(LogTemp, Warning, TEXT("조건 NPC 일치 → 조건 대사 로딩됨"));
                    break;
                }
            }
        }

        // 2) 조건 대사가 없을 경우 완료 조건부터 체크
        if (LinesOfDialogue.Num() == 0)
        {
            AActor* Giver = CurrentStep.QuestGiver.Get();
            AActor* Completer = CurrentStep.CompletionGiver.Get();

            const bool bIsQuestAccepted = QuestSubsystem->IsQuestAccepted(QuestAsset->QuestID);
            const bool bIsStepCompleted = QuestSubsystem->IsStepCompleted(QuestAsset->QuestID, StepIndex);

            //  완료 조건 Fulfilled + NPC가 Completer → 완료 대사
            bool bAllConditionsMet = true;
            for (const UQuestCondition* Condition : CurrentStep.Conditions)
            {
                if (Condition && !Condition->IsFulfilled())
                {
                    bAllConditionsMet = false;
                    break;
                }
            }

            if (bAllConditionsMet && Owner == Completer)
            {
                LinesOfDialogue = CurrentStep.CompleteDialogueLines;
                UE_LOG(LogTemp, Warning, TEXT(" CompletionGiver + 조건 충족 → 완료 대사 로딩됨"));
            }
            // 조건 Fulfilled가 아니고, 아직 Step 완료도 아니며 NPC가 Giver일 경우 → 수락 대사
            else if (!bIsStepCompleted && !QuestSubsystem->IsQuestCompleted(QuestAsset->QuestID) && Owner == Giver)
            {
                LinesOfDialogue = CurrentStep.GiverDialogueLines;
                UE_LOG(LogTemp, Warning, TEXT(" Giver + 미완료 Step → 수락 대사 로딩됨"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT(" 조건 미충족 or NPC 불일치 → 기본 대사 생략"));
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT(" 최종 LinesOfDialogue.Num(): %d"), LinesOfDialogue.Num());

    if (bResetDialogueIndex)
    {
        CurrentDialogueIndex = 0;
        bDialogueFinished = false;
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
    }
}
void UDialogueComponent::Interact()
{
    AEmberCharacter* Player = Cast<AEmberCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (Player)
    {
        // 나를 Player의 ActiveDialogueComponent 로 등록
        Player->SetActiveDialogueComponent(this);
    }
    if (QuestAsset)
    {
        if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
        {
            if (Subsystem->IsQuestCompleted(QuestAsset->QuestID))
            {
                UE_LOG(LogTemp, Warning, TEXT("[Interact] 퀘스트가 이미 완료됨 → 상호작용 차단"));
                return;
            }
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("[Interact] bDialogueOverriddenByCondition: %d, NumLines: %d"), bDialogueOverriddenByCondition, LinesOfDialogue.Num());

    if (!bPlayerInRange || !DialogueWidgetClass || bDialogueFinished)
        return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AActor* OwnerActor = GetOwner();

    if (!PC || !OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueComponent::Interact] Missing PC or Owner"));
        return;
    }

    //  조건 Fulfilled 여부 검사 (수락 상태에서만)
    if (bIsAccepteing && QuestAsset)
    {
        if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
        {
            const int32 StepIndex = Subsystem->GetCurrentStepIndexForQuest(QuestAsset->QuestID, false);
            if (QuestAsset->Steps.IsValidIndex(StepIndex))
            {
                const FQuestStep& Step = QuestAsset->Steps[StepIndex];

                bool bAllConditionsMet = true;
                for (const UQuestCondition* Cond : Step.Conditions)
                {
                    if (Cond && !Cond->IsFulfilled())
                    {
                        bAllConditionsMet = false;
                        break;
                    }
                }

                if (!bAllConditionsMet)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[Interact] 수락 중 + 조건 미충족 → 대화 차단"));
                    return;
                }
            }
        }
    }

    //  조건 통과 시 대화 진행
    if (!bDialogueOverriddenByCondition)
    {
        LoadDialogueFromDataTable(true);
    }

    bDialogueFinished = false;
    CurrentDialogueIndex = 0;
    PositionDetachedCamera();

    if (Player && Player->GetMesh())
    {
        Player->GetMesh()->SetVisibility(false, true);
    }

    DialogueWidget = CreateWidget<UUserWidget>(PC, DialogueWidgetClass);
    if (DialogueWidget)
    {
        const int32 HighZOrder = 999;
        DialogueWidget->AddToViewport(HighZOrder);

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
        AdvanceDialogue();
    }
}

void UDialogueComponent::AdvanceDialogue()
{

    UE_LOG(LogTemp, Warning, TEXT(">>>> AdvanceDialogue() called, Index: %d, NumLines: %d"), CurrentDialogueIndex, LinesOfDialogue.Num());

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

    // --- 대사 출력 ---
    if (CurrentDialogueIndex < LinesOfDialogue.Num())
    {
        if (UTextBlock* ChatBox = Cast<UTextBlock>(DialogueWidget->GetWidgetFromName("NPC_ChatBox")))
        {
            ChatBox->SetText(LinesOfDialogue[CurrentDialogueIndex]);
            UE_LOG(LogTemp, Warning, TEXT("[AdvanceDialogue] Line %d shown."), CurrentDialogueIndex + 1);
            ++CurrentDialogueIndex;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[AdvanceDialogue] NPC_ChatBox not found."));
        }
        return;
    }

    // --- 대사 다 봤을 때 ---
    bDialogueFinished = true;
    bDialogueOverriddenByCondition = false;
    UE_LOG(LogTemp, Warning, TEXT("[AdvanceDialogue] Dialogue finished. Checking quest logic."));

    UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    if (!QuestSubsystem || !QuestAsset)
    {
        SetDialogueVisualState(false);
        return;
    }

    // StepIndex 구하고 기본 분기
    const int32 StepIndex = QuestSubsystem->GetCurrentStepIndexForQuest(QuestAsset->QuestID, false);
    const int32 LastStepIndex = QuestAsset->Steps.Num() - 1;


    AActor* OwnerActor = GetOwner();
    int32 CurrentStepIndex = QuestSubsystem->GetCurrentStepIndexForQuest(QuestAsset->QuestID);

    // StepIndex가 -1 (미수락 상태)이거나, QuestGiver와 일치하는 경우
    if (StepIndex == -1 ||(QuestAsset->Steps.IsValidIndex(StepIndex) && OwnerActor == QuestAsset->Steps[StepIndex].QuestGiver.Get() &&CurrentStepIndex == StepIndex &&!bIsAccepteing))
    {
        UE_LOG(LogTemp, Warning, TEXT("[AdvanceDialogue] 현재 Step %d이 수락되지 않은 상태이거나 이 NPC가 QuestGiver임 → 수락 UI 표시"), StepIndex);
        ShowQuestUI();
        return;
    }
    const FQuestStep& CurrentStep = QuestAsset->Steps[StepIndex];
  
    // --- 조건 NPC(이벤트 발생) 처리 ---
    for (UQuestCondition* Condition : CurrentStep.Conditions)
    {
        if (UDialogueQuestCondition* DialogueCond = Cast<UDialogueQuestCondition>(Condition))
        {
            if (DialogueCond->TargetNPCActor.IsValid() && DialogueCond->TargetNPCActor.Get() == GetOwner())
            {
                FGameplayEventData EventData;
                EventData.EventTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.Dialogue.DialogueReceiver"));
                EventData.Instigator = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
                EventData.Target = GetOwner();

                if (UGameplayEventSubsystem* EventSystem = UGameplayEventSubsystem::GetGameplayEvent(GetWorld()))
                {
                    EventSystem->OnGameEvent.Broadcast(EventData.EventTag, EventData);
                    UE_LOG(LogTemp, Warning, TEXT(" [AdvanceDialogue] 조건 NPC 이벤트 Broadcast: %s"), *GetOwner()->GetName());
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT(" GameplayEventSubsystem을 찾을 수 없습니다."));
                }

                SetDialogueVisualState(false);
                return;
            }
        }
    }

    // --- 퀘스트 조건 확인 ---
    bool bAllConditionsMet = true;
    for (const UQuestCondition* Condition : CurrentStep.Conditions)
    {
        if (Condition && !Condition->IsFulfilled())
        {
            bAllConditionsMet = false;
            break;
        }
    }

    if (bAllConditionsMet)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AdvanceDialogue] 모든 조건 충족 → 퀘스트 완료 위젯 표시"));
        ShowQuestCompleteWidget(QuestAsset, true);
        return;
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
        SetInputMappingContexts({ GameplayInputMappingContext, GameplayUIInputMappingContext, GameplayAlsUIInputMappingContext }, true);
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
    QuestWidget = QuestUI;
    bool bIsComplete = false;
    bool bIsAccepted = false;

    if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
        bIsComplete = QuestSubsystem->IsQuestCompleted(QuestAsset->QuestID);
        bIsAccepted = QuestSubsystem->IsQuestAccepted(QuestAsset->QuestID);
    }

    QuestUI->SetQuestInfoFromDataAsset(QuestAsset, bIsComplete, bIsAccepted);

    QuestUI->OnQuestAccepted.AddLambda([this, QuestUI]()
        {
            if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
            {
                Subsystem->TryStartQuest(QuestAsset->QuestID, true);

                AcceptedStepIndex = Subsystem->GetCurrentStepIndexForQuest(QuestAsset->QuestID);
                if (AcceptedStepIndex == INDEX_NONE)
                {
                    AcceptedStepIndex = 0;
                }
            }
            ShowQuestTracker(false, AcceptedStepIndex);

            QuestUI->RemoveFromParent();
            bDialogueFinished = false;
            this->bIsAccepteing = true;
            SetDialogueVisualState(false);
        });

    QuestUI->OnQuestRefused.AddLambda([this, QuestUI]() {
        QuestUI->RemoveFromParent();
        SetDialogueVisualState(false);
        bDialogueFinished = false;
        });

    InitializeAndDisplayWidget(QuestUI);
}


void UDialogueComponent::ShowQuestCompleteWidget(const UQuestDataAsset* InQuestAsset, bool bIsQuestComplete)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC || !QuestCompleteWidgetClass || !QuestAsset) return;

    UQuestWidget* CompleteWidget = CreateWidget<UQuestWidget>(PC, QuestCompleteWidgetClass);

    if (!CompleteWidget) return;
    QuestCompleteWidget = CompleteWidget;
    if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
        const bool bIsComplete = QuestSubsystem->IsQuestCompleted(QuestAsset->QuestID);
        const bool bIsAccepted = QuestSubsystem->IsQuestAccepted(QuestAsset->QuestID);

        CompleteWidget->SetQuestInfoFromDataAsset(QuestAsset, bIsComplete, bIsAccepted);
    }


    CompleteWidget->OnQuestCompleted.AddLambda([this, PC, CapturedQuestAsset = QuestAsset, CompleteWidget]()
        {
            if (UQuestSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
            {
                //  완료 버튼 클릭 시 다음 퀘스트 스텝으로 진행
                Subsystem->AdvanceQuestStep(CapturedQuestAsset->QuestID);
            }
            if (PC && QuestTrackerClass)
            {
                UQuestTracker* Tracker = CreateWidget<UQuestTracker>(PC, QuestTrackerClass);
                if (Tracker)
                {
                    Tracker->AddToViewport();
                    ShowQuestTracker(true, AcceptedStepIndex);
                }
            }
            //  UI 제거
            if (CompleteWidget && CompleteWidget->IsInViewport())
            {
                CompleteWidget->RemoveFromParent();
            }

            //  입력 및 시점 복구
            SetDialogueVisualState(false);
            bDialogueFinished = false;
            this->bIsAccepteing = false;
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

void UDialogueComponent::ShowQuestTracker(bool bIsComplete, int32 StepIndex)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC || !QuestTrackerClass || !QuestAsset) return;

    UQuestTracker* Tracker = CreateWidget<UQuestTracker>(PC, QuestTrackerClass);
    if (!Tracker) return;

    Tracker->AddToViewport();

    const FQuestStep& Step = QuestAsset->Steps[StepIndex];
    FText StepType = Step.StepQuesttype;
    FText StepName = Step.StepName;

    UQuestCondition* FirstCondition = nullptr;
    if (Step.Conditions.Num() > 0)
    {
        FirstCondition = Step.Conditions[0];
    }

    Tracker->ShowTracker(StepType, StepName, FirstCondition, bIsComplete);
}

bool UDialogueComponent::IsDialogueActive() const
{
    return DialogueWidget != nullptr;
}
void UDialogueComponent::ResetDialogueState()
{
    LinesOfDialogue.Empty();
    CurrentDialogueIndex = 0;
    bDialogueFinished = false;
    bDialogueOverriddenByCondition = false;
}


void UDialogueComponent::CloseAnyOpenUI()
{
    //  위젯 체크
    if (QuestWidget)
    {
        QuestWidget->RemoveFromParent();
        QuestWidget = nullptr;
    }
    if (DialogueWidget)
    {
        DialogueWidget->RemoveFromParent();
        DialogueWidget = nullptr;
    }
    if (QuestCompleteWidget)
    {
        QuestCompleteWidget->RemoveFromParent();
        QuestCompleteWidget = nullptr;
    }
    // 입력 모드 & 카메라 복귀
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        PC->SetInputMode(FInputModeGameOnly());
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            PC->SetViewTargetWithBlend(PlayerPawn, 0.5f);
        }
        Player->GetMesh()->SetVisibility(true, true);
        SetInputMappingContexts({ GameplayInputMappingContext, GameplayUIInputMappingContext, GameplayAlsUIInputMappingContext }, true);
        PC->bShowMouseCursor = false;
    }
    ResetDialogueState();

}

void UDialogueComponent::Interact_Implementation(AActor* Caller)
{
    Interact();
}

float UDialogueComponent::GetGatherTime_Implementation()
{
    return 0.0f;
}

