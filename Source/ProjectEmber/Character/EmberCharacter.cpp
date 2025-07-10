
#include "EmberCharacter.h"
#include <Item/Core/EmberItemDeveloperSetting.h>
#include <Item/UI/SlotWidget/SlotsPanel/Implements/EmberBasePanel/EmberQuickSlotPanel.h>

#include "Item/UI/SlotWidget/Slot/EmberQuickSlotWidget.h"
#include "EmberAbilitySystem/Attribute/Character/EmberCharacterAttributeSet.h"
#include "InputHandler/EmberInputHandlerComponent.h"
#include "EmberComponents/InteractionComponent.h"
#include "ALSCamera/Public/AlsCameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "AlsCharacterMovementComponent.h"
#include "Framework/EmberPlayerState.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayEffect.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "GameInstance/EmberGameInstance.h"
#include "TargetSystemComponent.h"
#include "Animation/AnimInstance.h"
#include "UI/EmberHpBarUserWidget.h"
#include "Build/AC_BuildComponent.h"
#include "Define/CharacterDefine.h"
#include "EmberLog/EmberLog.h"
#include "Engine/LocalPlayer.h"
#include "FunctionLibrary/UIFunctionLibrary.h"
#include "GameInstance/EffectManagerSubsystem.h"
#include "Item/UserItemManger.h"
#include "Item/Craft/EmberCraftComponent.h"
#include "MeleeTrace/Public/MeleeTraceComponent.h"
#include "Quest/QuestSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Utility/AlsVector.h"
#include "MotionWarpingComponent.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "Components/WidgetComponent.h"
#include "UI/Death/DeathScreenWidget.h"
#include "AI_NPC/NPC_Component/DialogueComponent.h"
#include "Item/ItemSubsystem.h"
#include "Item/Core/ItemSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EmberCharacter)

AEmberCharacter::AEmberCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	OverlayStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OverlayStaticMesh"));
	OverlayStaticMesh->SetupAttachment(GetMesh());
	OverlayStaticMesh->SetRelativeLocation(FVector::ZeroVector);
	OverlayStaticMesh->SetRelativeRotation(FRotator::ZeroRotator);

	OverlaySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("OverlaySkeletalMesh"));
	OverlaySkeletalMesh->SetupAttachment(GetMesh());
	OverlaySkeletalMesh->SetRelativeLocation(FVector::ZeroVector);
	OverlaySkeletalMesh->SetRelativeRotation(FRotator::ZeroRotator);

	WeaponTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponTrail"));
	WeaponTrailComponent->SetupAttachment(GetMesh());
	WeaponTrailComponent->SetRelativeLocation(FVector::ZeroVector);
	WeaponTrailComponent->SetRelativeRotation(FRotator::ZeroRotator);
	WeaponTrailComponent->bAutoActivate = false;

	DualWeaponTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DualWeaponTrail"));
	DualWeaponTrailComponent->SetupAttachment(GetMesh());
	DualWeaponTrailComponent->SetRelativeLocation(FVector::ZeroVector);
	DualWeaponTrailComponent->SetRelativeRotation(FRotator::ZeroRotator);
	DualWeaponTrailComponent->bAutoActivate = false;


	Camera = CreateDefaultSubobject<UAlsCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh());
	Camera->SetRelativeRotation_Direct(FRotator(0.0f, 90.0f, 0.0f));

	InputHandler = CreateDefaultSubobject<UEmberInputHandlerComponent>(TEXT("InputHandler"));

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	MeleeTraceComponent = CreateDefaultSubobject<UMeleeTraceComponent>(TEXT("MeleeTraceComponent"));

	EmberItemManager = CreateDefaultSubobject<UUserItemManger>(TEXT("UserItemComponent"));
	CraftCollision = CreateDefaultSubobject<UEmberCraftComponent>(TEXT("CraftBoxCollision"));
	CraftCollision->SetRelativeLocation(FVector::ZeroVector);
	CraftCollision->SetRelativeRotation(FRotator::ZeroRotator);
	CraftCollision->SetupAttachment(GetMesh());

	VisualCharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_LittleBoyRyan"));
	VisualCharacterMesh->SetupAttachment(GetMesh());

	GliderMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Glide"));
	GliderMesh->SetupAttachment(GetMesh());

	BuildComponent = CreateDefaultSubobject<UAC_BuildComponent>(TEXT("BuildComponent"));

	MotionWarpComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpComponent"));

	TargetSystemComponent = CreateDefaultSubobject<UTargetSystemComponent>(TEXT("TargetSystemComponent"));

	PrimaryActorTick.bCanEverTick = true;

	ActiveDialogueComponent = nullptr;

	/*IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green,
				FString::Printf(TEXT("OnlineSubsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
		}
	}*/
}

void AEmberCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (BuildComponent)
	{
		BuildComponent->Camera = Camera;
	}
	if (UEmberGameInstance* GI = GetGameInstance<UEmberGameInstance>())
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC && PC->IsLocalController())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(GI->PlayerMappingContext, 0);
				Subsystem->AddMappingContext(GI->UI_ALS_MappingContext, 1);
				Subsystem->AddMappingContext(GI->UIMappingContext, 2);
				UE_LOG(LogTemp, Warning, TEXT("[Character::BeginPlay] MappingContext applied!"));

				GI->ApplySavedMoveBindingsToUserSettings();
				GI->ApplySavedActionKeyMappingsToUserSettings();
			}
		}
	}

	if (GetController()->IsLocalController())
	{
		if (AEmberPlayerState* EmberPlayerState = GetPlayerState<AEmberPlayerState>())
		{
			AbilitySystemComponent = EmberPlayerState->GetAbilitySystemComponent();
			Super::SetAbilitySystemComponent(AbilitySystemComponent);
			EmberItemManager->InitAbilitySystem();
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
			for (const auto& StartInputAbility : StartBlockInputAbilities)
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

		if (MeleeTraceComponent)
		{
			MeleeTraceComponent->OnTraceHit.AddDynamic(this, &AEmberCharacter::HandleMeleeTraceHit);
		}
	}

	MessageDelegateHandle = FMessageDelegate::CreateUObject(this, &ThisClass::ReceiveMessage);
	UMessageBus::GetInstance()->Subscribe(TEXT("DeathEnemy"), MessageDelegateHandle);

	TargetSystemComponent->OnTargetLockedOff.AddDynamic(this, &ThisClass::OnTargetLockedOff);

	if (HpBarWidget.IsValid())
	{
		HpBarWidget->SetAbilitySystemComponent(this);
	}
}

void AEmberCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateGlideMovement(DeltaSeconds);
	
	if (!TargetSystemComponent->IsLocked() && HitActors.Num() > 0)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(HitTimerHandle))
		{
			TargetSystemComponent->TargetLockOff(false);
			HitActors.Empty();
		}
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

void AEmberCharacter::OnOutOfHealth(AActor* InstigatorActor)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UUserWidget* DeathWidget = UUIFunctionLibrary::PushContentToLayer(PlayerController,
			FGameplayTag::RequestGameplayTag("UI.Layer.Modal"), DeathWidgetClass);
		
		Cast<UDeathScreenWidget>(DeathWidget)->SetOwner(this);
		UUIFunctionLibrary::FocusUI(PlayerController,DeathWidget,true,true,true);
	}
	
	/*GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(PlayerController);
	}*/
}

void AEmberCharacter::AbilityInputPressed(int32 InputID)
{
	if (UUIFunctionLibrary::GetIsAbilityInputLock(Cast<APlayerController>(GetController())) ||
		AbilitySystemComponent->HasMatchingGameplayTag(AlsLocomotionModeTags::Gliding))
	{
		return;
	}

	bool bActive = false;

	if (InputID == 0 && !AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::HeavyAttack))
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
	else if (InputID == 1 && !AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::WeakAttack))
	{
		if (FGameplayAbilitySpec* Spec = GetSpecFromOverlayMode(true))
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
	/*else
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
	}*/

	/* 콤보 공격을 이어주기 위함. 기존 Input Ability 들은 인식되지만
	 * 콤보 공격은 Input으로 이어지는게 아니다 보니 찾아서 호출시켜줘야됨
	 */
	if (bActive == false)
	{
		/** LMB, RMB가 서로 간섭하는걸 막기 위함 */
		if ((InputID == 1 && AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::WeakAttack)) ||
			(InputID == 0 && AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::HeavyAttack)))
		{
			return;
		}
		if (InputID == 1 && AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::WeakAttack))
		{
			EMBER_LOG(LogEmber, Warning, TEXT("InputID: %d , Tag: %s "), InputID, *AlsCharacterStateTags::WeakAttack.GetTag().ToString());	
		}
		else if (InputID == 0 && AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::HeavyAttack))
		{
			EMBER_LOG(LogEmber, Warning, TEXT("InputID: %d , Tag: %s "), InputID, *AlsCharacterStateTags::HeavyAttack.GetTag().ToString());	
		}
		
		for (auto& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (Spec.InputID == INDEX_NONE && Spec.IsActive())
			{
				AbilitySystemComponent->AbilitySpecInputPressed(Spec);
			}
		}
	}
  
	if (InputID == 1 && GetOverlayMode() == AlsOverlayModeTags::Hammer)
	{
		BuildComponent->RepairBuilding();
	}
	if (InputID == 0 && GetOverlayMode() == AlsOverlayModeTags::Hammer)
	{
		BuildComponent->SpwanBuild();
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
	else if (OverlayMode == AlsOverlayModeTags::Throw)
	{
		InputID = static_cast<int32>(EInputID::Throw);
	}
	else if (OverlayMode == AlsOverlayModeTags::Hammer)
	{
		InputID = static_cast<int32>(EInputID::Hammer);
	}

	if (IsRightInput)
	{
		InputID += 1000;
	}

	ensureMsgf(InputID != 0, TEXT("%s: GetSpecFromOverlayMode produced InputID == 0 (OverlayMode=%s, IsRightInput=%d)"),
	           *GetName(), *OverlayMode.ToString(), IsRightInput);

	return AbilitySystemComponent->FindAbilitySpecFromInputID(InputID);
}

FGameplayAbilitySpec* AEmberCharacter::GetBlockSpecFromOverlayMode() const
{
	int32 InputID = 0;
	if (OverlayMode == AlsOverlayModeTags::Default)
	{
		InputID = static_cast<int32>(EAimInputID::Default);
	}
	else if (OverlayMode == AlsOverlayModeTags::Sword)
	{
		InputID = static_cast<int32>(EAimInputID::Sword);
	}
	else if (OverlayMode == AlsOverlayModeTags::Hatchet)
	{
		InputID = static_cast<int32>(EAimInputID::Hatchet);
	}
	else if (OverlayMode == AlsOverlayModeTags::PickAxe)
	{
		InputID = static_cast<int32>(EAimInputID::PickAxe);
	}
	else if (OverlayMode == AlsOverlayModeTags::Spear)
	{
		InputID = static_cast<int32>(EAimInputID::Spear);
	}
	else if (OverlayMode == AlsOverlayModeTags::Dagger)
	{
		InputID = static_cast<int32>(EAimInputID::Dagger);
	}
	else if (OverlayMode == AlsOverlayModeTags::SwordTwoHanded)
	{
		InputID = static_cast<int32>(EAimInputID::SwordTwoHanded);
	}
	else if (OverlayMode == AlsOverlayModeTags::Throw)
	{
		InputID = static_cast<int32>(EAimInputID::Throw);
	}
	else if (OverlayMode == AlsOverlayModeTags::Hammer)
	{
		InputID = static_cast<int32>(EAimInputID::Hammer);
	}

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

void AEmberCharacter::TryAbilityFromOnBlock(const bool bPressed)
{
	if (FGameplayAbilitySpec* Spec = GetBlockSpecFromOverlayMode())
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

/*void AEmberCharacter::HandleMeleeTraceHit(UMeleeTraceComponent* ThisComponent, AActor* HitActor,
    const FVector& HitLocation, const FVector& HitNormal, FName HitBoneName, FMeleeTraceInstanceHandle TraceHandle)
{
    
}*/

UMeleeTraceComponent* AEmberCharacter::GetMeleeTraceComponent() const
{
	return MeleeTraceComponent;
}

void AEmberCharacter::SetHpBarWidget(UEmberHpBarUserWidget* InHpBarWidget)
{
	HpBarWidget = InHpBarWidget;
}

UNiagaraComponent* AEmberCharacter::GetWeaponTrailComponent() const
{
	return WeaponTrailComponent;
}

UNiagaraComponent* AEmberCharacter::GetDualWeaponTrailComponent() const
{
	return DualWeaponTrailComponent;
}

UNiagaraSystem* AEmberCharacter::GetOverlayHitEffect() const
{
	return OverlayHitEffect;
}

void AEmberCharacter::SetOverlayHitEffect(UNiagaraSystem* InHitEffectAsset)
{
	OverlayHitEffect = InHitEffectAsset;
}

void AEmberCharacter::PlayHitEffectAtLocation(const FVector& Location)
{
	UWorld* World = GetWorld();

	if (!OverlayHitEffect || !World)
	{
		return;
	}

	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UEffectManagerSubsystem* EffectManager = GameInstance->GetSubsystem<UEffectManagerSubsystem>())
		{
			EffectManager->PlayHitEffectAtLocation(OverlayHitEffect, Location,
			                                       FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true);
		}
	}
}

USkeletalMeshComponent* AEmberCharacter::GetGliderMesh() const
{
	return GliderMesh;
}

UTargetSystemComponent* AEmberCharacter::GetTargetSystemComponent() const
{
	return TargetSystemComponent;
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
		NewPC->InputYawScale_DEPRECATED = 1.0f;
		NewPC->InputPitchScale_DEPRECATED = 1.0f;
		NewPC->InputRollScale_DEPRECATED = 1.0f;

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

	if (GetCancelAbilityInput() && !AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::Blocking))
	{
		const FGameplayTagContainer CancelTags(AlsInputActionTags::OverlayAction);
		AbilitySystemComponent->CancelAbilities(&CancelTags);
	}
	
	const auto Value = UAlsVector::ClampMagnitude012D(ActionValue.Get<FVector2D>());
	const auto ForwardDir = UAlsVector::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw));
	const auto RightDir = UAlsVector::PerpendicularCounterClockwiseXY(ForwardDir);
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
	if (OverlayMode == AlsOverlayModeTags::Bow ||
		OverlayMode == AlsOverlayModeTags::Throw)
	{
		SwitchOnAimTarget(ActionValue.Get<bool>());
		
		TryAbilityFromOnAim(ActionValue.Get<bool>());
		SetDesiredAiming(ActionValue.Get<bool>());
	}
}

void AEmberCharacter::SwitchOnAimTarget(const bool bPressed)
{
	if (bPressed)
	{
		if (TargetSystemComponent->IsLocked())
		{
			CachedTargetActor = TargetSystemComponent->GetLockedOnTargetActor();
			TargetSystemComponent->TargetLockOff();
		}
	}
	else
	{
		if (CachedTargetActor.IsValid())
		{
			TargetSystemComponent->TargetActor({CachedTargetActor.Get()});
		}
		
		CachedTargetActor.Reset();
	}
}

void AEmberCharacter::Input_OnBlock(const FInputActionValue& ActionValue)
{
	TryAbilityFromOnBlock(ActionValue.Get<bool>());
}

void AEmberCharacter::Input_OnGlide()
{
	if (!AlsCharacterMovement)
	{
		return;
	}

	const FGameplayTag CurrentMode = GetLocomotionMode();

	if (CurrentMode == AlsLocomotionModeTags::InAir)
	{
		PreOverlayTag = GetOverlayMode();
		SetOverlayMode(AlsOverlayModeTags::Default);

		GliderMesh->SetHiddenInGame(false);

		SetLocomotionMode(AlsLocomotionModeTags::Gliding);

		AlsCharacterMovement->GravityScale = GlideGravityScale;

		const FRotator ControlRot = GetActorRotation();//GetControlRotation();
		const FVector ForwardDir = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::X);

		FVector NewVelocity = ForwardDir.GetSafeNormal() * GlideForwardSpeed;

		NewVelocity.Z = -FMath::Abs(GlideDescendSpeed);

		AlsCharacterMovement->Velocity = NewVelocity;
	}
	else if (CurrentMode == AlsLocomotionModeTags::Gliding)
	{
		SetLocomotionMode(AlsLocomotionModeTags::InAir);

		AlsCharacterMovement->GravityScale = DefaultGravityScale;
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

	if (AbilitySystemComponent->HasMatchingGameplayTag(AlsOverlayModeTags::Sword) ||
		GetOverlayMode() == AlsOverlayModeTags::Sword)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(DodgeAbilityClass);
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
	const FGameplayTagContainer CancelTags(AlsInputActionTags::OverlayAction);
	AbilitySystemComponent->CancelAbilities(&CancelTags);
	AbilitySystemComponent->CancelAllAbilities();
	AbilitySystemComponent->RemoveLooseGameplayTag(AlsInputActionTags::LockMoveInput);
	return;
	/*EMBER_LOG(LogEmber, Warning, TEXT("PressedIndex : %d"), PressedIndex);

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
	}*/
}

void AEmberCharacter::HandleMeleeTraceHit(UMeleeTraceComponent* ThisComponent, AActor* HitActor,
                                          const FVector& HitLocation, const FVector& HitNormal, FName HitBoneName,
                                          FMeleeTraceInstanceHandle TraceHandle)
{
	if (!HitActor)
	{
		return;
	}

	UAbilitySystemComponent* TargetAsc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
	if (!TargetAsc)
	{
		if (GetOverlayMode() == AlsOverlayModeTags::Hammer  ||
			GetOverlayMode() == AlsOverlayModeTags::PickAxe ||
			GetOverlayMode() == AlsOverlayModeTags::Hatchet)
		{
			PlayHitEffectAtLocation(HitLocation);
		}
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(ThisComponent);

	FHitResult HitResult;
	HitResult.ImpactPoint = HitLocation;
	HitResult.ImpactNormal = HitNormal;
	HitResult.BoneName = HitBoneName;
	EffectContext.AddHitResult(HitResult, true);

	if (DamageGameplayEffectClass)
	{
		if (const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			DamageGameplayEffectClass, MeleeTraceComponent->AttackLevel, EffectContext); SpecHandle.IsValid())
		{
			/* 여기서 타겟시스템 연동처리 0.5초안에 추가로 들어온 HitActor가 있는지
			 * 혼자면 그 HitActor에 대해서만 적용 만약 여러개가 있다면 가장 가까운 HitActor에 대해서만 적용
			 * TargetSystemComponent의 TargetActor함수를 인자값을 추가 후
			 * 타이머로 발동으로 처리
			 */
			if (!HitActors.Contains(HitActor))
			{
				HitActors.Add(HitActor);
			}
			
			if (!TargetSystemComponent->IsLocked())
			{
				if (!GetWorld()->GetTimerManager().IsTimerActive(HitTimerHandle))
				{
					/*GetWorld()->GetTimerManager().SetTimer(HitTimerHandle, this,
														   &AEmberCharacter::DelayToSendHitActors,
														   0.5f, false);*/
				}
			}

			EMBER_LOG(LogEmber, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
			
			AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetAsc);
			PlayHitEffectAtLocation(HitLocation);
		}
	}

	if (ABaseAIAnimal* BaseAIAnimal = Cast<ABaseAIAnimal>(HitActor))
	{
		BaseAIAnimal->SetFlash(0.5f);
	}
}

void AEmberCharacter::DelayToSendHitActors()
{
	TargetSystemComponent->TargetActor(HitActors);
}

void AEmberCharacter::ReceiveHitActorDeath(AActor* InstigatorActor)
{
	if (!InstigatorActor)
	{
		return;
	}
	
	const int32 NumRemoved = HitActors.Remove(InstigatorActor);
	if (NumRemoved > 0)
	{
		EMBER_LOG(LogEmber, Warning, TEXT("Removed %d dead hit actors."), NumRemoved);
		if (InstigatorActor == TargetSystemComponent->GetLockedOnTargetActor())
		{
			if (HitActors.Num() > 0)
			{
				TargetSystemComponent->TargetLockOff(false);
				GetController()->ResetIgnoreLookInput();
				
				GetWorld()->GetTimerManager().SetTimer(HitDeathTimerHandle, this,
															   &AEmberCharacter::DelayToSendHitActors,
															   0.2f, false);
				
			}
			else
			{
				TargetSystemComponent->TargetLockOff(false);
				GetController()->ResetIgnoreLookInput();
			}
		}
	}
}

void AEmberCharacter::OnTargetLockedOff(AActor* TargetActor)
{
	const int32 NumRemoved = HitActors.Remove(TargetActor);
	if (NumRemoved > 0)
	{
		if (HitActors.Num() > 0)
		{
			TargetSystemComponent->TargetLockOff(false);
			GetController()->ResetIgnoreLookInput();
			//TargetSystemComponent->TargetActor(HitActors);
		}
		else
		{
			GetController()->ResetIgnoreLookInput();
			TargetSystemComponent->TargetLockOff(false);
		}
	}
}

void AEmberCharacter::ReceiveMessage(const FName MessageType, UObject* Payload)
{
	if (MessageType == TEXT("DeathEnemy"))
	{
		if (AActor* InstigatorActor = Cast<AActor>(Payload))
		{
			ReceiveHitActorDeath(InstigatorActor);
		}
	}
}

UUserItemManger* AEmberCharacter::GetItemManager()
{
	return EmberItemManager;
}

UEmberCraftComponent* AEmberCharacter::GetCraftComponent()
{
	return CraftCollision;
}

void AEmberCharacter::Input_OnBuild()
{
	if (GetOverlayMode() != AlsOverlayModeTags::Hammer)
	{
		PreOverlayTag = GetOverlayMode();
		SetOverlayMode(AlsOverlayModeTags::Hammer);

		if (BuildComponent)
		{
			BuildComponent->LaunchBuildMode();
		}
	}
	else
	{
		SetOverlayMode(PreOverlayTag);
		PreOverlayTag = AlsOverlayModeTags::Default;

		if (BuildComponent)
		{
			BuildComponent->LaunchBuildMode();
		}
	}
}

void AEmberCharacter::OnResetTarget()
{
	HitActors.Empty();
	TargetSystemComponent->TargetLockOff(false);
	bIsHitActorsInputReset = true;
}

void AEmberCharacter::Input_OnStartTarget(const FInputActionValue& ActionValue)
{
	GetWorld()->GetTimerManager().SetTimer(HitTimerInputHandle, this, &ThisClass::OnResetTarget, 0.5f, false);
}

void AEmberCharacter::Input_OnSwitchTarget(const FInputActionValue& ActionValue)
{
	if (bIsHitActorsInputReset)
	{
		bIsHitActorsInputReset = false;
		return;
	}
	else
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(HitTimerInputHandle))
		{
			TimerManager.ClearTimer(HitTimerInputHandle);
		}
	}
	
	if (!TargetSystemComponent->IsLocked())
	{
		return;
	}

	AActor* CurrentTarget = TargetSystemComponent->GetLockedOnTargetActor();
	ensureMsgf(CurrentTarget, TEXT("%s: TargetActor is null when switching target!"), *GetName());
	if (!CurrentTarget)
	{
		return;
	}

	int32 CurrentIndex = HitActors.Find(CurrentTarget);
	if (CurrentIndex == INDEX_NONE)
	{
		HitActorsIndex = 0;
		return;
	}
	
	if (HitActorsIndex == CurrentIndex)
	{
		HitActorsIndex++;
	}
	
	if (HitActors.Num() == 0)
	{
		TargetSystemComponent->TargetLockOff();
		return;
	}
	
	if (HitActorsIndex < 0 || HitActorsIndex >= HitActors.Num())
	{
		HitActorsIndex = 0;
	}
	
	if (GetDistanceTo(HitActors[HitActorsIndex]) > TargetSystemComponent->MinimumDistanceToEnable)
	{
		HitActors.RemoveAt(HitActorsIndex);
		
		if (HitActorsIndex >= HitActors.Num())
		{
			HitActorsIndex = 0;
		}
	}
	
	TargetSystemComponent->TargetLockOff(false);
	TargetSystemComponent->TargetActor({HitActors[HitActorsIndex]});

	HitActorsIndex++;
}

void AEmberCharacter::Input_OnStartThrowQuick(const FInputActionValue& ActionValue)
{
	// 오버레이랑 통합시켜야됨
}

void AEmberCharacter::Input_OnCancelThrowQuick(const FInputActionValue& ActionValue)
{
	// 오버레이랑 통합시켜야됨
}

void AEmberCharacter::Input_OnSwitchThrowOverlay(const FInputActionValue& ActionValue)
{
	if (AbilitySystemComponent->HasMatchingGameplayTag(AlsLocomotionModeTags::Gliding))
	{
		return;
	}
	
	FGameplayTagContainer RequiredTags;
	RequiredTags.AddTag(AlsCharacterStateTags::Attack);
	RequiredTags.AddTag(AlsCharacterStateTags::ComboAttack);
	if (AbilitySystemComponent->HasAnyMatchingGameplayTags(RequiredTags))
	{
		return;
	}

	if (GetOverlayMode() == AlsOverlayModeTags::Throw)
	{
		SetOverlayMode(PreThrowOverlayTag);
	}
	else
	{
		PreThrowOverlayTag = GetOverlayMode();
		SetOverlayMode(AlsOverlayModeTags::Throw);
	}
}

void AEmberCharacter::ShowQuickActionWidget()
{
	//퀵슬롯 위젯 보이게 하기
	EMBER_LOG(LogEmber, Warning, TEXT("QuickActionTimerHandle ShowQuickActionWidget!"));
	bShowQuickActionWidget = true;
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(QuickActionTimerHandle);
	
	const UEmberItemDeveloperSetting* ItemSetting = UEmberItemDeveloperSetting::Get();

	if (!QuickSlotWidget.IsValid())
	{
		QuickSlotWidget = UItemSystemLibrary::GetItemSubsystem()->GetQuickSlotWidget();
			/*UUIFunctionLibrary::PushContentToLayer(Cast<APlayerController>(GetController()),
											   FGameplayTag::RequestGameplayTag("UI.Layer.Modal"),
											   ItemSetting->QuickSlotWidgetClass);*/
	}

	QuickSlotWidget->SetVisibility(ESlateVisibility::Visible);
	UUIFunctionLibrary::FocusUI(Cast<APlayerController>(GetController()),Cast<UUserWidget>(QuickSlotWidget),true, true, true);
}

void AEmberCharacter::Input_OnStartItemQuick(const FInputActionValue& ActionValue)
{
	EMBER_LOG(LogEmber, Warning, TEXT("QuickActionTimerHandle TimerSet!"));
	GetWorld()->GetTimerManager().SetTimer(QuickActionTimerHandle, this, &ThisClass::ShowQuickActionWidget, 1.0f, false);
}

void AEmberCharacter::Input_OnCancelItemQuick(const FInputActionValue& ActionValue)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	// 타이머가 발동중이면 (1초가 안지났다는 뜻)
	if (TimerManager.IsTimerActive(QuickActionTimerHandle))
	{
		TimerManager.ClearTimer(QuickActionTimerHandle);

		if (QuickSlotWidget.IsValid())
		{
			
			Cast<UEmberQuickSlotWidget>(Cast<UEmberQuickSlotPanel>(QuickSlotWidget)->Slots[HoveredSlotIndex])->UseQuickSlotItem(HoveredSlotIndex);
			
			EMBER_LOG(LogEmber, Warning, TEXT("QuickActionTimerHandle is active, using focused item."));
		}
	}
	// 퀵슬롯 위젯이 노출 중이면
	else if (bShowQuickActionWidget)
	{
		// 포커스 중인 슬롯으로 세팅 후 Widget 닫기
		bShowQuickActionWidget = false;
		if (QuickSlotWidget.IsValid())
		{
			if (UEmberQuickSlotPanel* SlotPanelWidget = Cast<UEmberQuickSlotPanel>(QuickSlotWidget))
			{
				for (TObjectPtr<UEmberBaseSlotWidget> Slot :SlotPanelWidget->Slots)
				{
					if (Cast<UEmberQuickSlotWidget>(Slot)->bIsHovered)
					{
						HoveredSlotIndex = Cast<UEmberQuickSlotWidget>(Slot)->SlotIndex;
						EMBER_LOG(LogEmber, Warning, TEXT("QuickActionWidget is showing, setting focused slot and closing widget."));
						
				 		break;
					}
				}
			}
		}
		
		UUIFunctionLibrary::FocusGame(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		QuickSlotWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AEmberCharacter::Input_OnStartScan(const FInputActionValue& ActionValue)
{
}

void AEmberCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused,
                                   float& VerticalLocation)
{
	if (Camera->IsActive())
		Camera->DisplayDebug(Canvas, DisplayInfo, VerticalLocation);

	Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
}

void AEmberCharacter::NotifyLocomotionModeChanged(const FGameplayTag& PreviousLocomotionMode)
{
	Super::NotifyLocomotionModeChanged(PreviousLocomotionMode);

	if (PreviousLocomotionMode == AlsLocomotionModeTags::Gliding)
	{
		AlsCharacterMovement->GravityScale = DefaultGravityScale;
		SetOverlayMode(PreOverlayTag);
		GliderMesh->SetHiddenInGame(true);
	}
}

bool AEmberCharacter::StartMantlingInAir()
{
	return false;
}

void AEmberCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(HitTimerHandle);

	UMessageBus::GetInstance()->Unsubscribe(TEXT("DeathEnemy"), MessageDelegateHandle);

	Super::EndPlay(EndPlayReason);
}

void AEmberCharacter::SetActiveDialogueComponent(UDialogueComponent* InDialogue)
{
	ActiveDialogueComponent = InDialogue;

	if (ActiveDialogueComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EmberCharacter] ActiveDialogueComponent set to: %s"), *ActiveDialogueComponent->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[EmberCharacter] ActiveDialogueComponent cleared."));
	}
}

void AEmberCharacter::Input_OnCloseDialogue()
{
	if (ActiveDialogueComponent)
	{
		ActiveDialogueComponent->CloseAnyOpenUI();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Input_OnCloseDialogue] No ActiveDialogueComponent to close!"));
	}
}

void AEmberCharacter::UpdateGlideMovement(float DeltaTime)
{
	if (LocomotionMode != AlsLocomotionModeTags::Gliding)
	{
		return;
	}

	const float DeadZoneSqr = 0.1f * 0.1f;
	
	// 상승기류
    if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::Updrafting))
    {
        FVector WorldInput = GetLastMovementInputVector();
        
        if (WorldInput.SizeSquared() > DeadZoneSqr)
        {
            WorldInput.Z = 0.f;
        	
            const FVector LocalInput = GetActorRotation().UnrotateVector(WorldInput).GetSafeNormal2D();
            const float InputYawOffset = FMath::RadiansToDegrees(FMath::Atan2(LocalInput.Y, LocalInput.X));
            const float NewYaw = FMath::FInterpTo(GetActorRotation().Yaw, GetActorRotation().Yaw + InputYawOffset,
                DeltaTime,1.f);
        	
            SetActorRotation(FRotator(0.f, NewYaw, 0.f));
        	
            const FVector ForwardDir = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
            FVector NewVel = ForwardDir * GlideForwardSpeed;
            NewVel.Z = AlsCharacterMovement->Velocity.Z;
            AlsCharacterMovement->Velocity = NewVel;
        }
        return;
    }

	// 일반
    FVector WorldInput = GetLastMovementInputVector();
    if (WorldInput.SizeSquared() <= DeadZoneSqr)
    {
        return;
    }

    WorldInput.Z = 0.f;
    FVector LocalInput = GetActorRotation().UnrotateVector(WorldInput).GetSafeNormal2D();
    float InputYawOffset = FMath::RadiansToDegrees(FMath::Atan2(LocalInput.Y, LocalInput.X));

    float CurrentYaw = GetActorRotation().Yaw;
    float DesiredYaw = CurrentYaw + InputYawOffset;
    float NewYaw = FMath::FInterpTo(CurrentYaw,DesiredYaw,DeltaTime,1.f);

    SetActorRotation(FRotator(0.f, NewYaw, 0.f));

    const FVector ForwardDir = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
    FVector NewVelocity = ForwardDir * GlideForwardSpeed;
    NewVelocity.Z = -FMath::Abs(GlideDescendSpeed);

    AlsCharacterMovement->Velocity = NewVelocity;
}

