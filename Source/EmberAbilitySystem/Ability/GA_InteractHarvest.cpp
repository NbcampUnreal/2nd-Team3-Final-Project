#include "GA_InteractHarvest.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_InteractHarvest::UGA_InteractHarvest()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_InteractHarvest::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);

	/* 어빌리티 호출 예시
	Harvest.OnDestroyed.AddDynamic(this, 동물::다먹었다);

	FGameplayEventData Payload;
	Payload.EventTag = 먹기태그;
	Payload.OptionalObject = HarvestActor;

	AbilitySystemComponent->HandleGameplayEvent(먹기태그, Payload);
	*/
	
	if (TriggerEventData && TriggerEventData->OptionalObject)
	{
		HarvestTarget = Cast<AActor>(TriggerEventData->OptionalObject.Get());
	}
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("LootHarvest"), Montage);
	
	Task->OnCompleted.AddDynamic(this, &UGA_InteractHarvest::OnCompleteCallback);
	Task->OnInterrupted.AddDynamic(this, &UGA_InteractHarvest::OnInterruptedCallback);
}

void UGA_InteractHarvest::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	
	if (AActor* HarvestActor = const_cast<AActor*>(HarvestTarget.Get()))
	{
		HarvestActor->OnDestroyed.Broadcast(HarvestActor);
		HarvestActor->Destroy();
	}
}

void UGA_InteractHarvest::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}