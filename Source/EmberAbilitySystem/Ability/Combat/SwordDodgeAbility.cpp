// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordDodgeAbility.h"

#include "AbilitySystemComponent.h"
#include "AlsCharacter.h"
#include "AlsCharacterMovementComponent.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/CapsuleComponent.h"

USwordDodgeAbility::USwordDodgeAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USwordDodgeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (const AAlsCharacter* Character = Cast<AAlsCharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (UAbilitySystemComponent* Asc = GetAbilitySystemComponentFromActorInfo())
		{
			UAnimMontage* MontageToPlay;
			if (Asc->HasMatchingGameplayTag(AlsRotationModeTags::Targeting))
			{
				SetUpdateWarping(false);
				
				const FVector2D MoveInput = Character->GetMoveInput();
				if (FMath::Abs(MoveInput.Y) > KINDA_SMALL_NUMBER)
				{
					MontageToPlay = (MoveInput.Y > 0.f) ? ForwardDodgeMontage : BackDodgeMontage;
				}
				else if (FMath::Abs(MoveInput.X) > KINDA_SMALL_NUMBER)
				{
					MontageToPlay = (MoveInput.X > 0.f) ? RightDodgeMontage : LeftDodgeMontage;
				}
				else
				{
					MontageToPlay = ForwardDodgeMontage;
				}
			}
			else
			{
				SetUpdateWarping(true);
				MontageToPlay = ForwardDodgeMontage;
			}
			
			if (MontageToPlay)
			{
				auto* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay);

				PlayMontageTask->OnCompleted.AddDynamic(this, &USwordDodgeAbility::OnMontageFinished);
				PlayMontageTask->OnInterrupted.AddDynamic(this, &USwordDodgeAbility::OnMontageFinished);
				PlayMontageTask->OnCancelled.AddDynamic(this, &USwordDodgeAbility::OnMontageFinished);

				PlayMontageTask->ReadyForActivation();
				return;
			}	
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
}

void USwordDodgeAbility::OnMontageFinished()
{
	if (const AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		auto* MoveComp = Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement());
		MoveComp->SetIsActiveOverlayAbility(false);	
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void USwordDodgeAbility::SetUpdateWarpingToTarget() const
{
	if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		FRotator CtrlRot = Character->GetController()->GetControlRotation();
		CtrlRot.Pitch = 0.f;
		CtrlRot.Roll  = 0.f;
		const FVector Forward = CtrlRot.Vector();
		const FVector Right   = FRotationMatrix(CtrlRot).GetScaledAxis(EAxis::Y);

		const FVector2D MoveInput = Character->GetMoveInput();
		FVector MoveDir;
		if (MoveInput.Size() <= KINDA_SMALL_NUMBER)
		{
			MoveDir = Character->GetActorForwardVector();
		}
		else
		{
			MoveDir = (Forward * MoveInput.Y + Right * MoveInput.X).GetSafeNormal();    
		}
		
		if (MoveDir.IsNearlyZero())
		{
			MoveDir = Forward;
		}
		
		const FVector DesiredWarp = Character->GetActorLocation() + MoveDir * WarpDistance;

		if (UMotionWarpingComponent* WarpComp = Character->FindComponentByClass<UMotionWarpingComponent>())
		{
			FMotionWarpingTarget WarpTarget;
			WarpTarget.Name     = FName("AttackWarp");
			WarpTarget.Location = DesiredWarp;
			WarpComp->AddOrUpdateWarpTarget(WarpTarget);
		}

		auto* MoveComp = Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement());
		MoveComp->SetIsActiveOverlayAbility(true);
		/*if (MontageToPlay == ForwardDodgeMontage)
		{
			
		}*/
	}
}

void USwordDodgeAbility::SetUpdateWarping(bool bIsForceRotation) const
{
	if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		FRotator CtrlRot = Character->GetController()->GetControlRotation();
		CtrlRot.Pitch = 0.f;
		CtrlRot.Roll  = 0.f;
		const FVector Forward = CtrlRot.Vector();
		const FVector Right   = FRotationMatrix(CtrlRot).GetScaledAxis(EAxis::Y);
		
		const FVector2D MoveInput = Character->GetMoveInput();
		FVector MoveDir;
		
		if (MoveInput.Size() <= 0.1f)
		{
			MoveDir = Character->GetActorForwardVector();
		}
		else
		{
			MoveDir = (Forward * MoveInput.Y + Right * MoveInput.X).GetSafeNormal();	
		}
		
		if (MoveDir.IsNearlyZero())
		{
			MoveDir = Forward;
		}
		
		const FVector DesiredWarp = Character->GetActorLocation() + MoveDir * WarpDistance;
		
		/*FHitResult Hit;
		FCollisionQueryParams Params(NAME_None, false, Character);
		const bool bBlocked = Character->GetWorld()->LineTraceSingleByChannel(Hit,Character->GetActorLocation(),
			DesiredWarp,
			ECC_EngineTraceChannel5,
			Params
		);*/
		
		//FVector FinalWarp = DesiredWarp;
		/*if (bBlocked)
		{
			const float Radius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
			FinalWarp = Hit.ImpactPoint - Hit.ImpactNormal * Radius;
		}*/

		if (UMotionWarpingComponent* WarpComp = Character->FindComponentByClass<UMotionWarpingComponent>())
		{
			FMotionWarpingTarget WarpTarget;
			WarpTarget.Name     = FName("AttackWarp");
			WarpTarget.Location = DesiredWarp;
			WarpTarget.Rotation = MoveDir.Rotation();
			WarpComp->AddOrUpdateWarpTarget(WarpTarget);
		}

		if (bIsForceRotation)
		{
			Character->ForceRoationTest(MoveDir.Rotation().Yaw);	
		}

		auto* MoveComp = Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement());
		MoveComp->SetIsActiveOverlayAbility(true);
	}
}
