
#include "BaseOverlayAbility.h"

#include "AbilitySystemComponent.h"
#include "AlsCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"

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

	LaunchCharacterForward(ActorInfo);

	if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->SetForceGameplayTags(ForceGameplayTags);
		PreLocomotionState = Character->GetLocomotionState();

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
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, MontageToPlay);

	PlayMontageTask->OnCompleted.AddDynamic(this, &UBaseOverlayAbility::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UBaseOverlayAbility::OnMontageInterrupted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UBaseOverlayAbility::OnMontageInterrupted);
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
	}
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
		AbilitySystemComponent->TryActivateAbilityByClass(NextComboAbility,false);
		
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

void UBaseOverlayAbility::LaunchCharacterForward(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (bEnableForwardMovementDuringMontage && ForwardMovementDistance > 0.f && ForwardMovementDuration > 0.f)
	{
		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			const FVector Forward = Character->GetActorForwardVector();
			const float Speed = ForwardMovementDistance / ForwardMovementDuration;
			Character->LaunchCharacter(Forward * Speed, true, true);
		}
	}
}

void UBaseOverlayAbility::OnMontageTick() const
{
	if (bMontageTickEnable)
	{
		if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
		{
			Character->ForceVelocityYawAngle(PreLocomotionState);
		}	
	}
}
