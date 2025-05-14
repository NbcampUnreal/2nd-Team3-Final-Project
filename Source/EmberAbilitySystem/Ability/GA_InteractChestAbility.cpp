#include "GA_InteractChestAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "EmberUtils/FunctionLibrary/UIFunctionLibrary.h"
#include "GameFramework/PlayerState.h"

UGA_InteractChestAbility::UGA_InteractChestAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_InteractChestAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* OwnerInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
    
    // 태그 초기화 (생성자 대신 필요 시)
    PickupTag = FGameplayTag::RequestGameplayTag("Character.Interact.Pickup");
    CloseTag  = FGameplayTag::RequestGameplayTag("Character.Interact.CloseChest");

    State = EChestState::Opening;
    PlayMontage(OpenMontage, FName("Open"));
}

void UGA_InteractChestAbility::PlayMontage(UAnimMontage* Montage, FName TaskName)
{
    if (!Montage)
        return;

    if (CurrentMontageTask)
    {
        CurrentMontageTask->EndTask();
        CurrentMontageTask = nullptr;
    }
    
    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage);
    CurrentMontageTask = Task;
    
    if (TaskName == "Open")
    {
        Task->OnCompleted.AddDynamic(this, &UGA_InteractChestAbility::OnOpenFinished);
        Task->OnInterrupted.AddDynamic(this, &UGA_InteractChestAbility::OnOpenFinished);
    }
    else if (TaskName == "Pick")
    {
        Task->OnCompleted.AddDynamic(this, &UGA_InteractChestAbility::OnPickFinished);
        Task->OnInterrupted.AddDynamic(this, &UGA_InteractChestAbility::OnPickFinished);
    }
    else if (TaskName == "Close")
    {
        Task->OnCompleted.AddDynamic(this, &UGA_InteractChestAbility::OnCloseFinished);
        Task->OnInterrupted.AddDynamic(this, &UGA_InteractChestAbility::OnCloseFinished);
    }

    Task->ReadyForActivation();
}

void UGA_InteractChestAbility::OnOpenFinished()
{
    State = EChestState::Opened;

    if (APawn* Pawn = Cast<APlayerState>(GetOwningActorFromActorInfo())->GetPawn())
    {
        // UI 표시 및 이동 잠금
        if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
        {
            UUserWidget* ChestWidgetInstance = UUIFunctionLibrary::PushContentToLayer(PC, FGameplayTag::RequestGameplayTag("UI.Layer.Modal"), ChestWidgetClass);

            if (const ACharacter* Character = Cast<ACharacter>(Pawn))
            {
                Character->GetCharacterMovement()->DisableMovement(); // 이동막기
                PC->SetIgnoreLookInput(true); // 카메라 막기
                PC->SetShowMouseCursor(true); // 커서 보이기

                //포커스 주기
                ChestWidgetInstance->SetUserFocus(PC);
                
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(ChestWidgetInstance->TakeWidget());
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                PC->SetInputMode(InputMode);
            }
        }

        // 픽업/닫기 이벤트 대기
        UAbilityTask_WaitGameplayEvent* PickupTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, PickupTag, nullptr, false);
        PickupTask->EventReceived.AddDynamic(this, &UGA_InteractChestAbility::OnPickupEvent);
        PickupTask->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* CloseTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CloseTag, nullptr, false);
        CloseTask->EventReceived.AddDynamic(this, &UGA_InteractChestAbility::OnCloseEvent);
        CloseTask->ReadyForActivation();
    }
    
}

void UGA_InteractChestAbility::OnPickupEvent(FGameplayEventData Payload)
{
    if (State != EChestState::Opened)
        return;

    State = EChestState::Picking;

    PlayMontage(PickupMontage, FName("Pick"));
}

void UGA_InteractChestAbility::OnPickFinished()
{
    State = EChestState::Opened;

}

void UGA_InteractChestAbility::OnCloseEvent(FGameplayEventData Payload)
{
    if (State == EChestState::Opened || State == EChestState::Picking)
    {
        State = EChestState::Closing;

        if (CurrentMontageTask)
        {
            CurrentMontageTask->EndTask();
            CurrentMontageTask = nullptr;
        }
    
        PlayMontage(CloseMontage, FName("Close"));    
    }
}

void UGA_InteractChestAbility::OnCloseFinished()
{
    //막은거 전부 복구
    if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
    {
        Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

        if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
        {
            PC->SetShowMouseCursor(false);
            PC->SetIgnoreLookInput(false);
            
            FInputModeGameOnly GameInput;
            PC->SetInputMode(GameInput);
        }
    }
    
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false,false);
}
