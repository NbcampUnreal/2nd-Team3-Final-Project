
#include "BaseOverlayAbility.h"

#include "AbilitySystemComponent.h"
#include "AlsCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "MessageBus/MessageBus.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBaseOverlayAbility::UBaseOverlayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBaseOverlayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	/* 모든 행동에대한 코스트, 쿨타임 등을 한번에 처리하고 bool 리턴해줌 */
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!bLoopingMontage)
	{
		SetUpdateWarping();
	}

	if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->SetForceGameplayTags(ForceGameplayTags);
		//Character->ForceLastInputDirectionBlocked(true);
		//PreLocomotionState = Character->GetLocomotionState();

		if (bMontageTickEnable)
		{
			Character->GetWorld()->GetTimerManager().SetTimer(
					MontageTickHandle,
					FTimerDelegate::CreateUObject(this, &UBaseOverlayAbility::OnMontageTick),
					0.033f,
					true
				  );	
		}
	}
	
	AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	UAnimMontage* SelectedMontage = ChooseMontageByState();
	if (!SelectedMontage)
	{
		SelectedMontage = DefaultMontage;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, SelectedMontage);

	PlayMontageTask->OnCompleted.AddDynamic(this, &UBaseOverlayAbility::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UBaseOverlayAbility::OnMontageInterrupted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UBaseOverlayAbility::OnMontageInterrupted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UBaseOverlayAbility::OnMontageCompleted);
	
	//PlayMontageTask->OnBlendOut.AddDynamic(this, &UBaseOverlayAbility::OnMontageCompleted);
	//PlayMontageTask->OnBlendOut
	PlayMontageTask->ReadyForActivation();
	
	if (!bLoopingMontage && bCanCombo)
	{
		static const FGameplayTag ComboTag = FGameplayTag::RequestGameplayTag(TEXT("Combat.Combo.Next"));
		UAbilityTask_WaitGameplayEvent* ComboTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, ComboTag, nullptr, false);
		
		ComboTask->EventReceived.AddDynamic(this, &UBaseOverlayAbility::OnComboNotify);
		ComboTask->ReadyForActivation();
	}
}

void UBaseOverlayAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UBaseOverlayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	bComboInputReceived = false;
	if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->ResetForceGameplayTags();
		if (!bLoopingMontage)
		{
			Character->SetDesiredGait(AlsGaitTags::Running);	
		}

		if (bMontageTickEnable)
		{
			Character->ForceLastInputDirectionBlocked(true);	
		}
	}

	if (auto AbilityClass = ChooseAbilityByState())
	{
		AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass,false);
	}
	
	UMessageBus::GetInstance()->BroadcastMessage(TEXT("OverlayAbilityEnded"), GetAvatarActorFromActorInfo());
}

void UBaseOverlayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	bComboInputReceived = true;
}

void UBaseOverlayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	bComboInputReceived = false;
	
	EndAbility(Handle, ActorInfo, ActivationInfo,true, true);
}

UAnimMontage* UBaseOverlayAbility::GetDefaultMontage() const
{
	return DefaultMontage;
}


void UBaseOverlayAbility::OnMontageCompleted()
{
	if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetWorld()->GetTimerManager().ClearTimer(MontageTickHandle);

		/*EMBER_LOG(LogEmber, Warning, TEXT("Changed VelocityYawAngle : %f"), Character->GetLocomotionState().VelocityYawAngle);
		EMBER_LOG(LogEmber, Warning, TEXT("Changed TargetYawAngle : %f"), Character->GetLocomotionState().TargetYawAngle);
		EMBER_LOG(LogEmber, Warning, TEXT("Changed InputYawAngle : %f"), Character->GetLocomotionState().InputYawAngle);*/
		//Character->SetRotationInstant(StartMontageActorYaw, ETeleportType::None);
	}
	
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);	
}

void UBaseOverlayAbility::OnMontageInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBaseOverlayAbility::OnComboNotify(const FGameplayEventData Payload)
{
	if (bCanCombo && bComboInputReceived)
	{
		if (!AbilitySystemComponent->TryActivateAbilityByClass(NextComboAbility,false))
		{
			EMBER_LOG(LogEmber,Warning, TEXT("Failed to activate next combo ability: %s"), *NextComboAbility->GetName());
		}
		
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

void UBaseOverlayAbility::OnMontageTick() const
{
	if (bMontageTickEnable)
	{
		if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
		{
			Character->ForceLastInputDirectionBlocked(true);
			//Character->ForceVelocityYawAngle(PreLocomotionState);
		}	
	}
}

void UBaseOverlayAbility::OnBlendOut()
{
}

void UBaseOverlayAbility::SetUpdateWarping()
{
	if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		FRotator CtrlRot = Character->GetController()->GetControlRotation();
		CtrlRot.Pitch = 0.f;
		CtrlRot.Roll  = 0.f;
		const FVector Forward = CtrlRot.Vector();
		const FVector Right   = FRotationMatrix(CtrlRot).GetScaledAxis(EAxis::Y);

		const FVector2D MoveInput = Character->GetMoveInput();
		FVector MoveDir = (Forward * MoveInput.Y + Right * MoveInput.X).GetSafeNormal();
		if (MoveDir.IsNearlyZero())
		{
			MoveDir = Forward;
		}
		
		const FVector DesiredWarp = Character->GetActorLocation() + MoveDir * WarpDistance;
		
		FHitResult Hit;
		FCollisionQueryParams Params(NAME_None, false, Character);
		const bool bBlocked = Character->GetWorld()->LineTraceSingleByChannel(Hit,Character->GetActorLocation(),
			DesiredWarp,
			ECC_Pawn,
			Params
		);
		
		FVector FinalWarp = DesiredWarp;
		if (bBlocked)
		{
			const float Radius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
			FinalWarp = Hit.ImpactPoint - Hit.ImpactNormal * Radius;
		}

		if (UMotionWarpingComponent* WarpComp = Character->FindComponentByClass<UMotionWarpingComponent>())
		{
			FMotionWarpingTarget WarpTarget;
			WarpTarget.Name     = FName("AttackWarp");
			WarpTarget.Location = FinalWarp;
			WarpTarget.Rotation = MoveDir.Rotation();
			WarpComp->AddOrUpdateWarpTarget(WarpTarget);
		}

		Character->ForceRoationTest(MoveDir.Rotation().Yaw);
	}
}

UAnimMontage* UBaseOverlayAbility::ChooseMontageByState()
{
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	}
	
	if (!AbilitySystemComponent || StateMontageMappings.Num() == 0)
	{
		return nullptr;
	}
	
	TArray<FStateMontageMapping> Sorted = StateMontageMappings;
	Sorted.Sort([](const FStateMontageMapping& A, const FStateMontageMapping& B)
	{
		return A.Priority > B.Priority;
	});

	for (const FStateMontageMapping& Mapping : Sorted)
	{
		if (AbilitySystemComponent->HasMatchingGameplayTag(Mapping.StateTag) && Mapping.Montage)
		{
			return Mapping.Montage;
		}
	}
	return nullptr;
}

TSubclassOf<UGameplayAbility> UBaseOverlayAbility::ChooseAbilityByState()
{
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	}
	
	if (!AbilitySystemComponent || StateAbilityMappings.Num() == 0)
	{
		return nullptr;
	}
	
	TArray<FStateAbilityMapping> Sorted = StateAbilityMappings;
	Sorted.Sort([](const FStateAbilityMapping& A, const FStateAbilityMapping& B)
	{
		return A.Priority > B.Priority;
	});

	for (const FStateAbilityMapping& Mapping : Sorted)
	{
		if (AbilitySystemComponent->HasMatchingGameplayTag(Mapping.StateTag) && Mapping.Ability)
		{
			return Mapping.Ability;
		}
	}
	
	return nullptr;
}
