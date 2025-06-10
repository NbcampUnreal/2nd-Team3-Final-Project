#include "EmberInputHandlerComponent.h"
#include "../EmberCharacter.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "AI_NPC/NPC_Component/DialogueComponent.h"
#include "Character/EmberComponents/InteractionComponent.h"
#include "UI/BaseWidget/GameMenuWidget.h"
#include "UI/HUD/EmberMainHUD.h"
#include "UI/Layer/EmberLayerBase.h"

UEmberInputHandlerComponent::UEmberInputHandlerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEmberInputHandlerComponent::BindInput(UEnhancedInputComponent* InputComponent)
{
    if (!InputComponent)
    {
        return;
    }
    InputComp = InputComponent;
    AEmberCharacter* Character = Cast<AEmberCharacter>(GetOwner());
    if (Character)
    {
        auto Bind = [&](UInputAction* Action, ETriggerEvent Trigger, auto Func)
        {
            if (IsValid(Action))
            {
                InputComponent->BindAction(Action, Trigger, Character, Func);
            }
        };

        // Look
        Bind(LookMouseAction,     ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnLookMouse);
        Bind(LookMouseAction,     ETriggerEvent::Canceled,   &AEmberCharacter::Input_OnLookMouse);
        Bind(LookAction,          ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnLook);
        Bind(LookAction,          ETriggerEvent::Canceled,   &AEmberCharacter::Input_OnLook);

        // Movement & actions
        Bind(MoveAction,          ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnMove);
        Bind(MoveAction,          ETriggerEvent::Canceled,   &AEmberCharacter::Input_OnMove);
        Bind(SprintAction,        ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnSprint);
        Bind(SprintAction,        ETriggerEvent::Canceled,   &AEmberCharacter::Input_OnSprint);
        Bind(WalkAction,          ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnWalk);
        Bind(CrouchAction,        ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnCrouch);
        Bind(JumpAction,          ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnJump);
        Bind(JumpAction,          ETriggerEvent::Canceled,   &AEmberCharacter::Input_OnJump);
        Bind(AimAction,           ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnAim);
        Bind(AimAction,           ETriggerEvent::Canceled,   &AEmberCharacter::Input_OnAim);
        Bind(RagdollAction,       ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnRagdoll);
        Bind(RollAction,          ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnRoll);
        //Bind(RotationModeAction,  ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnRotationMode);
        //Bind(ViewModeAction,      ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnViewMode);
        Bind(SwitchShoulderAction,ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnSwitchShoulder);
        Bind(GlideAction,         ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnGlide);
        Bind(BuildAction,         ETriggerEvent::Triggered,  &AEmberCharacter::Input_OnBuild);
        
        
        UInteractionComponent* Comp = Character->InteractionComponent.Get();
        if (Comp)
        {
            InputComponent->BindAction(InteractAction, ETriggerEvent::Started, Comp, &UInteractionComponent::Interact);
            InputComponent->BindAction(InteractAction, ETriggerEvent::Completed, Comp, &UInteractionComponent::StopGather);
            InputComponent->BindAction(InteractAction, ETriggerEvent::Started, Comp, &UInteractionComponent::TriggerAdvanceDialogue);
        }
    }

    int32 Index = 0;
    for (auto Action : QuickSlotActions)
    {
        if (IsValid(Action))
        {
            InputComponent->BindAction(Action, ETriggerEvent::Started, Character, &AEmberCharacter::Input_OnQuickSlot,Index++);
        }
    }
    // Ability input
    InputComponent->BindAction(AttackAction, ETriggerEvent::Started, Character, &AEmberCharacter::AbilityInputPressed, 0);


}

void UEmberInputHandlerComponent::RegisterMapping(APlayerController* PC, int32 Priority, const FModifyContextOptions& Options)
{
    if (!PC || !IsValid(InputMappingContext)) return;
    if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(InputMappingContext, Priority, Options);
        Subsystem->AddMappingContext(UIInputMappingContext, Priority, Options);
    }
}

void UEmberInputHandlerComponent::UnregisterMapping(APlayerController* PC)
{
    if (!PC || !IsValid(InputMappingContext)) return;
    if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
    {
        Subsystem->RemoveMappingContext(InputMappingContext);
        Subsystem->RemoveMappingContext(UIInputMappingContext);
    }
}

void UEmberInputHandlerComponent::BindUIInput(UGameMenuWidget* Layer)
{
    /* 임시 결국 옮겨야됨
     */
    AEmberCharacter* Character = Cast<AEmberCharacter>(GetOwner());
    if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
    {
        InputComp->BindAction(UIInventoryAction, ETriggerEvent::Started, Layer, &UGameMenuWidget::Input_ToggleInventory);
        InputComp->BindAction(UIQuestAction, ETriggerEvent::Started, Layer, &UGameMenuWidget::Input_ToggleQuest);
    }
}