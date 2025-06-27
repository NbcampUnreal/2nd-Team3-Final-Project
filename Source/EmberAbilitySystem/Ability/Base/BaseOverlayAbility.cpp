
#include "BaseOverlayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AlsCharacter.h"
#include "AlsCharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "MessageBus/MessageBus.h"
#include "MotionWarpingComponent.h"
#include "SkillManagerSubsystem.h"
#include "Attribute/Player/EmberPlayerAttributeSet.h"
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

	if (bIsBlockAbility)
	{
		UAbilitySystemComponent* Asc = GetAbilitySystemComponentFromActorInfo();
		
		Asc->AddLooseGameplayTag(AlsCharacterStateTags::Parrying);

		if (const UEmberPlayerAttributeSet* PlayerAttributesSet = Asc->GetSet<UEmberPlayerAttributeSet>())
		{
			GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().SetTimer(ParryingTimerHandle,
			FTimerDelegate::CreateUObject(this, &UBaseOverlayAbility::OnParryEnded),
			/* 추후 어트리뷰트에서 패링판정 시간을 가져와서 세팅해주기 */ PlayerAttributesSet->GetParryDuration(), 
			false
			);		
		}
	}
	
	if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->SetForceGameplayTags(ForceGameplayTags);
		//Character->ForceLastInputDirectionBlocked(true);
		//PreLocomotionState = Character->GetLocomotionState();
		if (!bIsBlockAbility)
		{
			Character->SetCancelAbilityInput(false);
		}
	
		if (!bLoopingMontage && bIsWarping && Character->GetLocomotionMode() != AlsLocomotionModeTags::InAir)
		{
			SetUpdateWarping();
		}
		
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
	/* Throw 가 쓰고 있음 주석 ㄴㄴ */
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UBaseOverlayAbility::OnMontageCompleted);
	
	PlayMontageTask->ReadyForActivation();
	
	if (!bLoopingMontage && bCanCombo)
	{
		static const FGameplayTag ComboTag = FGameplayTag::RequestGameplayTag(TEXT("Combat.Combo.Next"));
		UAbilityTask_WaitGameplayEvent* ComboTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, ComboTag, nullptr, false);
		
		ComboTask->EventReceived.AddDynamic(this, &UBaseOverlayAbility::OnComboNotify);
		ComboTask->ReadyForActivation();
	}

	if (bIsBlockAbility)
	{
		UAbilityTask_WaitGameplayEvent* BlockTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
	this, AlsCharacterStateTags::Hit, nullptr, false);
		
		BlockTask->EventReceived.AddDynamic(this, &UBaseOverlayAbility::OnBlockHit);
		BlockTask->ReadyForActivation();
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
	if (BlockHitMontageTask)
	{
		BlockHitMontageTask->EndTask();
		BlockHitMontageTask = nullptr;

		if (AActor* Avatar = GetAvatarActorFromActorInfo())
		{
			if (auto* Character = Cast<ACharacter>(Avatar))
			{
				if (auto* AnimInst = Character->GetMesh()->GetAnimInstance())
				{
					// 0.f 이면 즉시 컷, >0이면 BlendOutTime 만큼 페이드아웃
					AnimInst->Montage_Stop(0.25f, /* 몽타주 지정 안 하면 모두 중단 */ nullptr);
				}
			}
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (auto AbilityClass = ChooseAbilityByState())
	{
		AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass,false);
	}
	
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

	AbilitySystemComponent->RemoveLooseGameplayTag(AlsCharacterStateTags::Parrying);
	//AbilitySystemComponent->RemoveLooseGameplayTag(AlsCharacterStateTags::Blocking);
	
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
	
	if (GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().IsTimerActive(BlockHitTimerHandle))
	{
		bEndAfterBlockHit = true;
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

UAnimMontage* UBaseOverlayAbility::GetDefaultMontage() const
{
	return DefaultMontage;
}


void UBaseOverlayAbility::OnMontageCompleted()
{
	if (const AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetWorld()->GetTimerManager().ClearTimer(MontageTickHandle);
		
		auto* MoveComp = Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement());
		MoveComp->SetIsActiveOverlayAbility(false);
	}
	
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);	
}

void UBaseOverlayAbility::OnMontageInterrupted()
{
	if (!GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().IsTimerActive(BlockHitTimerHandle))
	{
		if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
		{
			auto* MoveComp = Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement());
			MoveComp->SetIsActiveOverlayAbility(false);
		}
	
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = true;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);	
	}
}

void UBaseOverlayAbility::OnComboNotify(const FGameplayEventData Payload)
{
	if (bCanCombo && bComboInputReceived)
	{
		USkillManagerSubsystem* SkillManagerSubsystem = GetAvatarActorFromActorInfo()->GetGameInstance()->GetSubsystem<USkillManagerSubsystem>();
		auto Abilities = SkillManagerSubsystem->GetNextComboAbilities(ThisClass::GetClass());
		// 나중에 좌클릭눌렷는지 우클릭눌렷는지 Received에서 판단해서 나누자
		if (Abilities.Num() > 0)
		{
			if (!AbilitySystemComponent->TryActivateAbilityByClass(Abilities[0],false))
			{
				EMBER_LOG(LogEmber,Warning, TEXT("Failed to activate next combo ability: %s"), *Abilities[0]->GetName());
			}
		}
		
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
	else if (bCanCombo && !bComboInputReceived)
	{
		if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
		{
			Character->SetCancelAbilityInput(true);
		}
	}
}

void UBaseOverlayAbility::OnMontageTick() const
{
	if (bMontageTickEnable)
	{
		if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
		{
			Character->ForceLastInputDirectionBlocked(true);
			//Character->GetCharacterMovement()->StopMovementImmediately();
			//Character->ForceVelocityYawAngle(PreLocomotionState);
		}	
	}
}

void UBaseOverlayAbility::OnBlendOut()
{
}

void UBaseOverlayAbility::OnParryEnded() const
{
	if (UAbilitySystemComponent* Asc = GetAbilitySystemComponentFromActorInfo_Ensured())
	{
		Asc->RemoveLooseGameplayTag(AlsCharacterStateTags::Parrying);
	}
}

void UBaseOverlayAbility::OnBlockHit(const FGameplayEventData Payload)
{
	if (!DefaultMontage)
		return;
	
	if (AAlsCharacter* Character = Cast<AAlsCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance())
		{
			AnimInst->Montage_SetNextSection(TEXT("Default"),TEXT("BlockHit"),
				DefaultMontage);

			AnimInst->Montage_SetNextSection(TEXT("BlockHit"),	TEXT("Default"),
				DefaultMontage);
		}
	}

	if (GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().IsTimerActive(BlockHitTimerHandle))
	{
		GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().ClearTimer(BlockHitTimerHandle);
	}
	
	float SectionLength = 0.f;
	const int32 SectionIndex = DefaultMontage->GetSectionIndex(TEXT("BlockHit"));
	if (SectionIndex != INDEX_NONE)
	{
		SectionLength = DefaultMontage->GetSectionLength(SectionIndex);
	}

	AbilitySystemComponent->AddLooseGameplayTag(AlsInputActionTags::LockMoveInput);
	
	BlockHitMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, DefaultMontage,1.f,TEXT("BlockHit"));
	BlockHitMontageTask->ReadyForActivation();

	// 5) 섹션 길이만큼 타이머 예약
	if (SectionLength > KINDA_SMALL_NUMBER)
	{
		GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().SetTimer(
			BlockHitTimerHandle,
			this,
			&UBaseOverlayAbility::OnBlockHitSectionTimerFinished,
			SectionLength,
			false
		);
	}
}

void UBaseOverlayAbility::OnBlockHitSectionTimerFinished()
{
	if (GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().IsTimerActive(BlockHitTimerHandle))
	{
		GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().ClearTimer(BlockHitTimerHandle);
	}

	AbilitySystemComponent->RemoveLooseGameplayTag(AlsInputActionTags::LockMoveInput);
	// 원래 블록히트 끝날 때 하던 로직
	// (여기서 기본 Default 섹션으로 이미 돌아가 있습니다)
	//bComboInputReceived = false;  // 혹 필요하면

	// InputReleased 중에 End 요청이 있었다면 여기서 끝내기
	if (bEndAfterBlockHit)
	{
		bEndAfterBlockHit = false;
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
	}
}

void UBaseOverlayAbility::SetUpdateWarping() const
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
		
		FHitResult Hit;
		FCollisionQueryParams Params(NAME_None, false, Character);
		const bool bBlocked = Character->GetWorld()->LineTraceSingleByChannel(Hit,Character->GetActorLocation(),
			DesiredWarp,
			ECC_EngineTraceChannel5,
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

		auto* MoveComp = Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement());
		MoveComp->SetIsActiveOverlayAbility(true);
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
