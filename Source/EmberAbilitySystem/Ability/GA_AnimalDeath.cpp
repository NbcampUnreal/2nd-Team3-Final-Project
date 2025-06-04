#include "GA_AnimalDeath.h"

#include "AbilitySystemComponent.h"
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
	
	if (!TriggerEventData)
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
		return;
	}

	//시체 바닥에 쓰러지는 사운드 큐 재생 -> BP에서 처리

	
	if (DeathEffectToApply)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DeathEffectToApply, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle EffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
	}
	//타이머 걸기 -> 20초 정도 죽은 상태로 보이면서 대기
	GetWorld()->GetTimerManager().SetTimer(WaitFarmingTimerHandle, this, &ThisClass::EndFarmingTime, 5.0f, false);
}


void UGA_AnimalDeath::EndFarmingTime() //파밍시간 종료 되면 호출될 함수
{
	AActor* Actor = GetAvatarActorFromActorInfo();
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = Actor;
	Param.Location = Actor->GetActorLocation();

	GetAbilitySystemComponentFromActorInfo_Ensured()->
		ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Animal.Death"), Param);
	
	EndCueDelegateHandle = GetAbilitySystemComponentFromActorInfo_Ensured()->
		RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("GameplayCue.Animal.Death"),
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_AnimalDeath::CallEndAbility);
	
	
}
void UGA_AnimalDeath::CallEndAbility(const FGameplayTag Tag, int32 NewCount)
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_AnimalDeath::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		// 델리게이트 언바인딩 (메모리 누수 방지)
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("GameplayCue.Animal.Death"), EGameplayTagEventType::NewOrRemoved)
		   .Remove(EndCueDelegateHandle);
	}
	
	//어빌리티 끝나면 필드에서 숨김처리 시키기
	UE_LOG(LogTemp, Warning, TEXT("[Death] Send HideAnimal for %s (%p)"), *GetAvatarActorFromActorInfo()->GetName(), GetAvatarActorFromActorInfo());
	UObject* HideAnimal = Cast<UObject>(GetAvatarActorFromActorInfo());
	UMessageBus::GetInstance()->BroadcastMessage(TEXT("HideAnimal"), HideAnimal);
}