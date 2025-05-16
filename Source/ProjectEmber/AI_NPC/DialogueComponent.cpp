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
#include "EngineUtils.h"

UDialogueComponent::UDialogueComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bPlayerInRange = false;
    DialogueCameraActor = nullptr;
    DialogueWidget = nullptr;
    InteractionTrigger = nullptr;
    TalkPromptWidget = nullptr;
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
    }

    if (InteractionTrigger)
    {
        InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &UDialogueComponent::OnPlayerEnterRadius);
        InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &UDialogueComponent::OnPlayerExitRadius);
    }
}

void UDialogueComponent::OnPlayerEnterRadius(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA<ACharacter>())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueComponent] Player entered NPC radius: %s"), *OtherActor->GetName());

        TalkPromptWidget->SetVisibility(true);
        bPlayerInRange = true;
    }
}

void UDialogueComponent::OnPlayerExitRadius(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (Cast<ACharacter>(OtherActor) && TalkPromptWidget)
    {
        TalkPromptWidget->SetVisibility(false);
        bPlayerInRange = false;

        ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (Player && Player->GetMesh())
        {
            Player->GetMesh()->SetVisibility(true, true);
            UE_LOG(LogTemp, Warning, TEXT("Player mesh restored"));
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
                FInputModeGameOnly InputMode;
                PC->SetInputMode(InputMode);
            }
        }
    }
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

    UE_LOG(LogTemp, Warning, TEXT("NPC rotated to face its own dialogue camera."));
}
void UDialogueComponent::PositionDetachedCamera()
{
    AActor* NPC = GetOwner();
    if (!NPC) return;

    UAttatchAIDialogueCamera* CameraComp = NPC->FindComponentByClass<UAttatchAIDialogueCamera>();
    if (!CameraComp) {
        UE_LOG(LogTemp, Warning, TEXT("No AttatchAIDialogueCamera found on NPC, using fallback camera."));
        return; 
    }
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
            UE_LOG(LogTemp, Warning, TEXT("ViewTarget set to cloned AttatchAIDialogueCamera"));
        }
    }
}
void UDialogueComponent::Interact()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (DialogueWidget)
    {
        DialogueWidget->RemoveFromParent();
        DialogueWidget = nullptr;

        if (TalkPromptWidget)
        {
            TalkPromptWidget->SetVisibility(true);
        }

        if (Player && Player->GetMesh())
        {
            Player->GetMesh()->SetVisibility(true, true);
        }

        if (PC)
        {
            PC->SetViewTargetWithBlend(PC->GetPawn(), 0.5f);
            PC->bShowMouseCursor = false;

            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
        }

        UE_LOG(LogTemp, Warning, TEXT("[DialogueComponent] Dialogue ended."));
        return;
    }

    if (bPlayerInRange && DialogueWidgetClass && PC && GetOwner())
    {
        RepositionNPCForDialogue();
        PositionDetachedCamera();

        if (Player && Player->GetMesh())
        {
            Player->GetMesh()->SetVisibility(false, true);
        }

        DialogueWidget = CreateWidget<UUserWidget>(PC, DialogueWidgetClass);
        if (DialogueWidget)
        {
            DialogueWidget->AddToViewport();
            PC->bShowMouseCursor = true;

            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(DialogueWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);

            if (TalkPromptWidget)
            {
                TalkPromptWidget->SetVisibility(false);
            }

            UE_LOG(LogTemp, Warning, TEXT("[DialogueComponent] Dialogue started."));
        }
    }
}

void UDialogueComponent::Interact_Implementation(AActor* Caller)
{
    Interact();
}

float UDialogueComponent::GetGatherTime_Implementation()
{
    return 0.0f;
}