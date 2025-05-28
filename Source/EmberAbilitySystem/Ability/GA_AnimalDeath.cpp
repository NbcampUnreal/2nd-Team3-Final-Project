#include "GA_AnimalDeath.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "MessageBus/MessageBus.h"

UGA_AnimalDeath::UGA_AnimalDeath()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_AnimalDeath::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	
	//if (OwnerInfo->OwnerActor)// TWeakObjectPtr<AActor>를 if 조건문에 직접 사용하는 것이 금지되었음을 의미
	
	if (OwnerInfo->OwnerActor.IsValid())
	{
		Instigator = OwnerInfo->OwnerActor;
	}

	if (TriggerEventData->OptionalObject)
	{
		Montage = const_cast<UAnimMontage*>(Cast<const UAnimMontage>(TriggerEventData->OptionalObject.Get()));
	}
	ensure(Montage);
	
	// 몽타주 재생
	UAbilityTask_PlayMontageAndWait* Task =	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("AnimalDeath"), Montage);
	if (Task)
	{
		Task->OnCompleted.AddDynamic(this, &UGA_AnimalDeath::OnCompleteCallback);
		Task->ReadyForActivation();
	}
}

void UGA_AnimalDeath::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	
	if (EffectToApply) //아직없음
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectToApply, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
	}
	UMessageBus::GetInstance()->BroadcastMessage(TEXT("HideAnimal"), this);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
