// ReSharper disable CppMemberFunctionMayBeConst
#include "EmberCharacter.h"
#include "EmberAbilitySystem/Attribute/Character/EmberCharacterAttributeSet.h"
#include "InputHandler/EmberInputHandlerComponent.h"
#include "EmberComponents/InteractionComponent.h"
#include "ALSCamera/Public/AlsCameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "AlsCharacterMovementComponent.h"
#include "Framework/EmberPlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "WaterBodyActor.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Define/CharacterDefine.h"
#include "EmberLog/EmberLog.h"
#include "Engine/LocalPlayer.h"
#include "FunctionLibrary/UIFunctionLibrary.h"
#include "GameFramework/PhysicsVolume.h"
#include "Item/UserItemManger.h"
#include "UI/EmberWidgetComponent.h"
#include "MeleeTrace/Public/MeleeTraceComponent.h"
#include "UI/HUD/EmberMainHUD.h"
#include "Utility/AlsVector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EmberCharacter)

AEmberCharacter::AEmberCharacter()
{
    Camera = CreateDefaultSubobject<UAlsCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(GetMesh());
    Camera->SetRelativeRotation_Direct(FRotator(0.0f, 90.0f, 0.0f));
    
    InputHandler = CreateDefaultSubobject<UEmberInputHandlerComponent>(TEXT("InputHandler"));
    
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

    MeleeTraceComponent = CreateDefaultSubobject<UMeleeTraceComponent>(TEXT("MeleeTraceComponent"));
    
    EmberItemManager = CreateDefaultSubobject<UUserItemManger>(TEXT("UserItemComponent"));
    
    HpBarWidget = CreateDefaultSubobject<UEmberWidgetComponent>(TEXT("HpBarWidget"));
    HpBarWidget->SetupAttachment(GetMesh());
    HpBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
    
    QuestReceiverComponent = CreateDefaultSubobject<UQuestReceiverComponent>(TEXT("QuestReceiverComponent"));
}

void AEmberCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if (GetController()->IsLocalController())
    {
        if (AEmberPlayerState* EmberPlayerState = GetPlayerState<AEmberPlayerState>())
        {
            AbilitySystemComponent = EmberPlayerState->GetAbilitySystemComponent();
            Super::SetAbilitySystemComponent(AbilitySystemComponent);
        
            if (const UEmberCharacterAttributeSet* CurrentAttributeSet = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
            {
                CurrentAttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
            }
        
            AbilitySystemComponent->InitAbilityActorInfo(EmberPlayerState, this);    
        
        
            for (const TSubclassOf<UGameplayAbility>& Ability : StartAbilities)
            {
                FGameplayAbilitySpec StartAbilitySpec = Ability;
                AbilitySystemComponent->GiveAbility(StartAbilitySpec);
            }

            for (const auto& StartInputAbility : StartInputAbilities)
            {
                FGameplayAbilitySpec StartAbilitySpec(StartInputAbility.Value);
                StartAbilitySpec.InputID = StartInputAbility.Key;
                AbilitySystemComponent->GiveAbility(StartAbilitySpec);
            }

            for (const auto& StartInputAbility : StartRightInputAbilities)
            {
                FGameplayAbilitySpec StartAbilitySpec(StartInputAbility.Value);
                StartAbilitySpec.InputID = StartInputAbility.Key;
                AbilitySystemComponent->GiveAbility(StartAbilitySpec);
            }
        
            /*TArray<FGameplayAbilitySpec> Specs = AbilitySystemComponent->GetActivatableAbilities();
            UE_LOG(LogEmber, Warning, TEXT("→ Specs 개수: %d"), Specs.Num());
            for (const FGameplayAbilitySpec& Spec : Specs)
            {
                UE_LOG(LogEmber, Warning, TEXT("→ Spec: %s"), *Spec.Ability->GetName());
            }*/
        
        
            /*APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
            PlayerController->ConsoleCommand(TEXT("ShowDebug AbilitySystem"));*/
            //bClientAbility = true;   
        }
    }
    
    if (HpBarWidgetClass)
    {
        HpBarWidget->SetWidgetClass(HpBarWidgetClass);
        HpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
        HpBarWidget->SetDrawSize(FVector2D(200.0f,20.0f));
        HpBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        
        HpBarWidget->UpdateAbilitySystemComponent(this);
    }
}

void AEmberCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    SetupEmberInputComponent();
}

UAbilitySystemComponent* AEmberCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AEmberCharacter::OnOutOfHealth()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        DisableInput(PlayerController);
    }
}

void AEmberCharacter::AbilityInputPressed(int32 InputID)
{
    if (UUIFunctionLibrary::GetIsAbilityInputLock(Cast<APlayerController>(GetController())))
    {
        return;
    }

    bool bActive = false;
    
    if (InputID == 0)
    {
        if (FGameplayAbilitySpec* Spec = GetSpecFromOverlayMode())
        {
            Spec->InputPressed = true;
            if (Spec->IsActive())
            {
                AbilitySystemComponent->AbilitySpecInputPressed(*Spec);
                bActive = true;
            }
            else
            {
                bActive = AbilitySystemComponent->TryActivateAbility(Spec->Handle);
            }
        }
    }
    else
    {
        if (FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromInputID(InputID))
        {
            Spec->InputPressed = true;
            if (Spec->IsActive())
            {
                AbilitySystemComponent->AbilitySpecInputPressed(*Spec);
                bActive = true;
            }
            else
            {
                bActive = AbilitySystemComponent->TryActivateAbility(Spec->Handle);
            }
        }
    }

    /* 콤보 공격을 이어주기 위함. 기존 Input Ability 들은 인식되지만
     * 콤보 공격은 Input으로 이어지는게 아니다 보니 찾아서 호출시켜줘야됨
     */
    if (bActive == false)
    {
        for (auto& Spec : AbilitySystemComponent->GetActivatableAbilities())
        {
            if (Spec.InputID == INDEX_NONE && Spec.IsActive())
            {
                AbilitySystemComponent->AbilitySpecInputPressed(Spec);
            }
        }
    }
}

FGameplayAbilitySpec* AEmberCharacter::GetSpecFromOverlayMode(const bool IsRightInput) const
{
    int32 InputID = 0;
    if (OverlayMode == AlsOverlayModeTags::Default)    
    {
        InputID = static_cast<int32>(EInputID::Default);
    }
    else if (OverlayMode == AlsOverlayModeTags::Sword) 
    {
        InputID = static_cast<int32>(EInputID::Sword);
    }
    else if (OverlayMode == AlsOverlayModeTags::Hatchet) 
    {
        InputID = static_cast<int32>(EInputID::Hatchet);
    }
    else if (OverlayMode == AlsOverlayModeTags::PickAxe) 
    {
        InputID = static_cast<int32>(EInputID::PickAxe);
    }
    else if (OverlayMode == AlsOverlayModeTags::Spear) 
    {
        InputID = static_cast<int32>(EInputID::Spear);
    }
    else if (OverlayMode == AlsOverlayModeTags::Dagger) 
    {
        InputID = static_cast<int32>(EInputID::Dagger);
    }
    else if (OverlayMode == AlsOverlayModeTags::SwordTwoHanded) 
    {
        InputID = static_cast<int32>(EInputID::SwordTwoHanded);
    }
    
    if (IsRightInput)
    {
        InputID += 1000;
    }

    ensureMsgf(InputID != 0, TEXT("%s: GetSpecFromOverlayMode produced InputID == 0 (OverlayMode=%s, IsRightInput=%d)"),
           *GetName(), *OverlayMode.ToString(), IsRightInput);
    
    return AbilitySystemComponent->FindAbilitySpecFromInputID(InputID);
}

void AEmberCharacter::TryAbilityFromOnAim(const bool bPressed)
{
    if (FGameplayAbilitySpec* Spec = GetSpecFromOverlayMode(true))
    {
        Spec->InputPressed = bPressed;

        if (Spec->IsActive())
        {
            if (bPressed)
            {
                AbilitySystemComponent->AbilitySpecInputPressed(*Spec);
            }
            else
            {
                AbilitySystemComponent->AbilitySpecInputReleased(*Spec);
            }
        }
        else if (bPressed)
        {
            AbilitySystemComponent->TryActivateAbility(Spec->Handle);
        }    
    }
}


UMeleeTraceComponent* AEmberCharacter::GetMeleeTraceComponent() const
{
    return MeleeTraceComponent;
}

void AEmberCharacter::NotifyControllerChanged()
{
    // 이전 컨트롤러 매핑 해제
    if (auto* PrevPC = Cast<APlayerController>(PreviousController))
    {
        InputHandler->UnregisterMapping(PrevPC);
    }

    // 새 컨트롤러 매핑 등록
    if (auto* NewPC = Cast<APlayerController>(GetController()))
    {
        NewPC->InputYawScale_DEPRECATED   = 1.0f;
        NewPC->InputPitchScale_DEPRECATED = 1.0f;
        NewPC->InputRollScale_DEPRECATED  = 1.0f;

        FModifyContextOptions Options;
        Options.bNotifyUserSettings = true;
        InputHandler->RegisterMapping(NewPC, 0, Options);
    }

    Super::NotifyControllerChanged();
}

void AEmberCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo)
{
    if (Camera->IsActive())
    {
        Camera->GetViewInfo(ViewInfo);
        return;
    }
    
    Super::CalcCamera(DeltaTime, ViewInfo);
}

void AEmberCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    SetupEmberInputComponent();
}

void AEmberCharacter::SetupEmberInputComponent() const
{
    if (IsValid(AbilitySystemComponent) && IsValid(InputComponent))
    {
        UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent);
    
        InputHandler->BindInput(EnhancedInput);
    }
}

void AEmberCharacter::BindUIInput(UGameMenuWidget* Layer)
{
    InputHandler->BindUIInput(Layer);
}

void AEmberCharacter::Input_OnLookMouse(const FInputActionValue& ActionValue)
{
    const FVector2f Value{ActionValue.Get<FVector2D>()};
    AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
    AddControllerYawInput(Value.X * LookRightMouseSensitivity);
}

void AEmberCharacter::Input_OnLook(const FInputActionValue& ActionValue)
{
    const FVector2f Value{ActionValue.Get<FVector2D>()};
    AddControllerPitchInput(Value.Y * LookUpRate);
    AddControllerYawInput(Value.X * LookRightRate);
}

void AEmberCharacter::Input_OnMove(const FInputActionValue& ActionValue)
{
    if (AbilitySystemComponent->HasMatchingGameplayTag(AlsInputActionTags::LockMoveInput) ||
        UUIFunctionLibrary::GetIsGameMovementInputLock(Cast<APlayerController>(GetController())))
    {
        return;
    }
    
    const auto Value = UAlsVector::ClampMagnitude012D(ActionValue.Get<FVector2D>());
    const auto ForwardDir = UAlsVector::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw));
    const auto RightDir   = UAlsVector::PerpendicularCounterClockwiseXY(ForwardDir);
    AddMovementInput(ForwardDir * Value.Y + RightDir * Value.X);
}

void AEmberCharacter::Input_OnSprint(const FInputActionValue& ActionValue)
{
    SetDesiredGait(ActionValue.Get<bool>() ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
}

void AEmberCharacter::Input_OnWalk()
{
    if (GetDesiredGait() == AlsGaitTags::Walking)
        SetDesiredGait(AlsGaitTags::Running);
    else if (GetDesiredGait() == AlsGaitTags::Running)
        SetDesiredGait(AlsGaitTags::Walking);
}

void AEmberCharacter::Input_OnCrouch()
{
    if (UUIFunctionLibrary::GetIsGameMovementInputLock(Cast<APlayerController>(GetController())))
    {
        return;
    }
    
    if (GetDesiredStance() == AlsStanceTags::Standing)
        SetDesiredStance(AlsStanceTags::Crouching);
    else
        SetDesiredStance(AlsStanceTags::Standing);
}

void AEmberCharacter::Input_OnJump(const FInputActionValue& ActionValue)
{
    if (AbilitySystemComponent->HasMatchingGameplayTag(AlsInputActionTags::LockJumping))
    {
        return;
    }
    
    if (ActionValue.Get<bool>())
    {
        if (GetCharacterMovement()->IsFalling())
        {
            if (Super::StartMantlingInAir())
            {
                return;
            }
        }
        
        if (StopRagdolling() || StartMantlingGrounded())
            return;        
        if (GetStance() == AlsStanceTags::Crouching)
        {
            SetDesiredStance(AlsStanceTags::Standing);
            return;
        }
        Jump();
    }
    else
    {
        StopJumping();
    }
}

void AEmberCharacter::Input_OnAim(const FInputActionValue& ActionValue)
{
    TryAbilityFromOnAim(ActionValue.Get<bool>());
    
    if (OverlayMode == AlsOverlayModeTags::Default)
    {
        
    }
    else if (OverlayMode == AlsOverlayModeTags::Bow)
    {
        SetDesiredAiming(ActionValue.Get<bool>());    
    }
}

void AEmberCharacter::Input_OnRagdoll()
{
    if (!StopRagdolling())
        StartRagdolling();
}

void AEmberCharacter::Input_OnRoll()
{
    if (AbilitySystemComponent->HasMatchingGameplayTag(AlsInputActionTags::LockRolling))
    {
        return;
    }

    const FGameplayTagContainer CancelTags(AlsInputActionTags::OverlayAction);
    AbilitySystemComponent->CancelAbilities(&CancelTags);
    
    StartRolling(1.3f);
}

void AEmberCharacter::Input_OnRotationMode()
{
    SetDesiredRotationMode(
        GetDesiredRotationMode() == AlsRotationModeTags::VelocityDirection
        ? AlsRotationModeTags::ViewDirection
        : AlsRotationModeTags::VelocityDirection);
}

void AEmberCharacter::Input_OnViewMode()
{
    SetViewMode(
        GetViewMode() == AlsViewModeTags::ThirdPerson
        ? AlsViewModeTags::FirstPerson
        : AlsViewModeTags::ThirdPerson);
}

void AEmberCharacter::Input_OnSwitchShoulder()
{
    Camera->SetRightShoulder(!Camera->IsRightShoulder());
}

void AEmberCharacter::Input_OnQuickSlot(int32 PressedIndex)
{
    EMBER_LOG(LogEmber, Warning, TEXT("PressedIndex : %d"), PressedIndex);
    
    FName SelectItemName = EmberItemManager->SelectQuickSlot(PressedIndex);
    if (SelectItemName.IsNone())
    {
        SetOverlayMode(AlsOverlayModeTags::Default);
        return;
    }

    const FString ItemNameStr = SelectItemName.ToString();

    if (ItemNameStr.Contains(TEXT("Sword")))
    {
        SetOverlayMode(AlsOverlayModeTags::Sword);
    }
    else if (ItemNameStr.Contains(TEXT("Bow")))
    {
        SetOverlayMode(AlsOverlayModeTags::Bow);
    }
    else if (ItemNameStr.Contains(TEXT("Hatchet")))
    {
        SetOverlayMode(AlsOverlayModeTags::Hatchet);
    }
    else if (ItemNameStr.Contains(TEXT("PickAxe")))
    {
        SetOverlayMode(AlsOverlayModeTags::PickAxe);
    }
    else // else는 잡템
    {
        SetOverlayMode(AlsOverlayModeTags::Default);
    }
}

void AEmberCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation)
{
    if (Camera->IsActive())
        Camera->DisplayDebug(Canvas, DisplayInfo, VerticalLocation);

    Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
}

bool AEmberCharacter::StartMantlingInAir()
{
    return false;
}
void AEmberCharacter::ToggleQuestUI()
{
    if (!QuestWidgetInstance)
    {
        QuestWidgetInstance = CreateWidget<UPlayerQuestWidget>(GetWorld(), QuestWidgetClass);
    }

    if (QuestWidgetInstance->IsInViewport())
    {
        QuestWidgetInstance->RemoveFromParent();
    }
    else
    {
        QuestWidgetInstance->AddToViewport(100);
        UE_LOG(LogTemp, Warning, TEXT(">>> Q 키 눌림 - 위젯 열기"));

        if (QuestReceiverComponent)
        {
            const FQuestDataRow& LastQuest = QuestReceiverComponent->GetLastAcceptedQuest();
            UE_LOG(LogTemp, Warning, TEXT(">>> 불러온 퀘스트 이름: %s"), *LastQuest.QuestName);
            bool bIsComplete = QuestReceiverComponent->IsQuestComplete(LastQuest.QuestID);
            QuestWidgetInstance->SetQuestInfoFromDataRow(LastQuest, bIsComplete);
        }
    }
}

