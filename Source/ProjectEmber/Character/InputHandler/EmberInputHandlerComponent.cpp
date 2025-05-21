#include "EmberInputHandlerComponent.h"
#include "../EmberCharacter.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "AI_NPC/DialogueComponent.h"
#include "Character/EmberComponents/InteractionComponent.h"
#include "UI/HUD/EmberMainHUD.h"

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

        UInteractionComponent* Comp = Character->InteractionComponent.Get();
        if (Comp)
        {
            InputComponent->BindAction(InteractAction, ETriggerEvent::Started, Comp, &UInteractionComponent::Interact);
            InputComponent->BindAction(InteractAction, ETriggerEvent::Completed, Comp, &UInteractionComponent::StopGather);
        }

        if (IsValid(NextDialogueAction) && Comp)
        {
            InputComponent->BindAction(NextDialogueAction, ETriggerEvent::Started, Comp, &UInteractionComponent::TriggerAdvanceDialogue);
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


    if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
    {
        AEmberMainHUD* HUD = Cast<AEmberMainHUD>(PlayerController->GetHUD());

        InputComponent->BindAction(UIInventoryAction, ETriggerEvent::Started, HUD, &AEmberMainHUD::Input_ToggleLayer, FGameplayTag::RequestGameplayTag(TEXT("UI.Layer.GameMenu")), FName(TEXT("Inventory")));
        InputComponent->BindAction(UIQuestAction, ETriggerEvent::Started, HUD, &AEmberMainHUD::Input_ToggleLayer, FGameplayTag::RequestGameplayTag(TEXT("UI.Layer.GameMenu")), FName(TEXT("Quest")));
    }
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
