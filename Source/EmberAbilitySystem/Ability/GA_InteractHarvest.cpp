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

	// 이벤트 페이로드에서 Destroy할 대상 추출
	if (TriggerEventData->OptionalObject)
	{
		HarvestTarget = Cast<AActor>(TriggerEventData->OptionalObject.Get());
	}

	// 몽타주 재생
	if (Montage)
	{
		UAbilityTask_PlayMontageAndWait* Task =	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("LootHarvest"), Montage);

		Task->OnCompleted.AddDynamic(this, &UGA_InteractHarvest::OnCompleteCallback);
		Task->OnInterrupted.AddDynamic(this, &UGA_InteractHarvest::OnInterruptedCallback);
		Task->ReadyForActivation();
	}
	else
	{
		OnCompleteCallback();
	}
}

void UGA_InteractHarvest::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
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

