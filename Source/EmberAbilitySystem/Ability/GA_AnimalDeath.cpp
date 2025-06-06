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

	/*
	 *ExecuteGameplayCue : 한 번의 이펙트(연출)만 실행-> 태그는 ASC에 등록되지 않음. -> 태그의 Count는 변하지 않음.
	 *그래서 EGameplayTagEventType(태그 카운트 변경) 이 되지 않아서 GameplayTagEvent 델리게이트는 호출되지 않았음.
	 *델리게이트 등록 -> 큐 연출 실행 -> 태그를 추가 Count 변화 발생 (0 → 1)
	 */

	EndCueDelegateHandle = GetAbilitySystemComponentFromActorInfo_Ensured()->
		RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("GameplayCue.Animal.Death"),
		EGameplayTagEventType::AnyCountChange).AddUObject(this, &UGA_AnimalDeath::CallEndAbility);
	
	GetAbilitySystemComponentFromActorInfo_Ensured()->
	ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Animal.Death"), Param);
	
	GetAbilitySystemComponentFromActorInfo_Ensured()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.Animal.Death"));
}

void UGA_AnimalDeath::CallEndAbility(const FGameplayTag Tag, int32 NewCount)
{
	GetAbilitySystemComponentFromActorInfo_Ensured()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.Animal.Death"));

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
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("GameplayCue.Animal.Death"), EGameplayTagEventType::AnyCountChange)
		   .Remove(EndCueDelegateHandle);
	}
	
	//어빌리티 끝나면 필드에서 숨김처리 시키기
	UE_LOG(LogTemp, Warning, TEXT("[Death] Send HideAnimal for %s (%p)"), *GetAvatarActorFromActorInfo()->GetName(), GetAvatarActorFromActorInfo());
	UObject* HideAnimal = Cast<UObject>(GetAvatarActorFromActorInfo());
	UMessageBus::GetInstance()->BroadcastMessage(TEXT("HideAnimal"), HideAnimal);
}