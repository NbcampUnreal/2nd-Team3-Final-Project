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
#include "Engine/LocalPlayer.h"
#include "GameFramework/PhysicsVolume.h"
#include "UI/EmberWidgetComponent.h"
#include "MeleeTrace/Public/MeleeTraceComponent.h"
#include "Utility/AlsVector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EmberCharacter)

AEmberCharacter::AEmberCharacter()
{
    Camera = CreateDefaultSubobject<UAlsCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(GetMesh());
    Camera->SetRelativeRotation_Direct(FRotator(0.0f, 90.0f, 0.0f));
    
    InputHandler = CreateDefaultSubobject<UEmberInputHandlerComponent>(TEXT("InputHandler"));

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
  
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

    MeleeTraceComponent = CreateDefaultSubobject<UMeleeTraceComponent>(TEXT("MeleeTraceComponent"));
    
    HpBarWidget = CreateDefaultSubobject<UEmberWidgetComponent>(TEXT("HpBarWidget"));
    HpBarWidget->SetupAttachment(GetMesh());
    HpBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

}

void AEmberCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HpBarWidgetClass)
    {
        HpBarWidget->SetWidgetClass(HpBarWidgetClass);
        HpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
        HpBarWidget->SetDrawSize(FVector2D(200.0f,20.0f));
        HpBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        HpBarWidget->UpdateAbilitySystemComponent();
    }

    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEmberCharacter::OnWaterBeginOverlap);
    GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AEmberCharacter::OnWaterEndOverlap);
}

void AEmberCharacter::OnWaterBeginOverlap(UPrimitiveComponent* OverlappedComp,
                             AActor* OtherActor,
                             UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex,
                             bool bFromSweep,
                             const FHitResult& SweepResult)
{
    // PhysicsVolume 기반 수영 모드 전환
    if (APhysicsVolume* Vol = Cast<APhysicsVolume>(OtherActor))
    {
        if (Vol->bWaterVolume)
            GetCharacterMovement()->SetMovementMode(MOVE_Swimming);
    }
    // 또는 WaterBody 액터 직접 체크
    else if (OtherActor->IsA<AWaterBody>())
    {
        GetCharacterMovement()->SetMovementMode(MOVE_Swimming);
    }
}

void AEmberCharacter::OnWaterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
    if (GetCharacterMovement()->MovementMode == MOVE_Swimming)
        GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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
    if (FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromInputID(InputID))
    {
        Spec->InputPressed = true;
        if (Spec->IsActive())
        {
            AbilitySystemComponent->AbilitySpecInputPressed(*Spec); 
        }
        else
        {
            AbilitySystemComponent->TryActivateAbility(Spec->Handle);
        }
    }
}

void AEmberCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (AEmberPlayerState* EmberPlayerState = GetPlayerState<AEmberPlayerState>())
    {
        AbilitySystemComponent = EmberPlayerState->GetAbilitySystemComponent();

        if (const UEmberCharacterAttributeSet* CurrentAttributeSet = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
        {
            CurrentAttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
        }
		
        if (AbilitySystemComponent)
        {
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

            SetupEmberInputComponent();

            /*APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
            PlayerController->ConsoleCommand(TEXT("ShowDebug AbilitySystem"));*/
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
    if (GetDesiredStance() == AlsStanceTags::Standing)
        SetDesiredStance(AlsStanceTags::Crouching);
    else
        SetDesiredStance(AlsStanceTags::Standing);
}

void AEmberCharacter::Input_OnJump(const FInputActionValue& ActionValue)
{
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
    SetDesiredAiming(ActionValue.Get<bool>());
}

void AEmberCharacter::Input_OnRagdoll()
{
    if (!StopRagdolling())
        StartRagdolling();
}

void AEmberCharacter::Input_OnRoll()
{
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

