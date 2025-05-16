#include "GA_InteractHarvest.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_InteractHarvest::UGA_InteractHarvest()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

/* 어빌리티 호출 예시
Harvest.OnDestroyed.AddDynamic(this, 동물::다먹었다);

FGameplayEventData Payload;
Payload.EventTag = 먹기태그;
Payload.OptionalObject = HarvestActor;

AbilitySystemComponent->HandleGameplayEvent(먹기태그, Payload);
*/

void UGA_InteractHarvest::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
	
	/* Event를 통한 Activation만 허용 (TryActivateAbility~ 로 들어오면 TriggerEventData가 Nullptr이다)
	 * HandleGameplayEvent 로 호출해야 파괴시킬 액터 담을 수 있음
	 */
	if (!TriggerEventData)
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
		return;
	}

	//이벤트 페이로드에서 Destroy할 대상 추출
	if (TriggerEventData->OptionalObject)
	{
		HarvestTarget = Cast<AActor>(TriggerEventData->OptionalObject2.Get());
	}

	if (TriggerEventData->Target)
	{
		HarvestTarget = TriggerEventData->Target;
	}
	
	//if (OwnerInfo->OwnerActor)// TWeakObjectPtr<AActor>를 if 조건문에 직접 사용하는 것이 금지되었음을 의미
	if (OwnerInfo->OwnerActor.IsValid())
	{
		Instigator = OwnerInfo->OwnerActor;
	}

	UAnimMontage* ResultMontage;
	if (TriggerEventData->OptionalObject2)
	{
		ResultMontage = const_cast<UAnimMontage*>(Cast<const UAnimMontage>(TriggerEventData->OptionalObject2.Get()));
	}
	else
	{
		ResultMontage = Montage;
	}

	ensure(ResultMontage);
	
	// 몽타주 재생
	UAbilityTask_PlayMontageAndWait* Task =	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("LootHarvest"), ResultMontage);

	Task->OnCompleted.AddDynamic(this, &UGA_InteractHarvest::OnCompleteCallback);
	Task->OnInterrupted.AddDynamic(this, &UGA_InteractHarvest::OnInterruptedCallback);
	Task->ReadyForActivation();
}

void UGA_InteractHarvest::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	
	if (EffectToApply)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectToApply, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	
	if (AActor* HarvestActor = const_cast<AActor*>(HarvestTarget.Get()))
	{
		HarvestActor->Destroy();
	}
}

void UGA_InteractHarvest::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

