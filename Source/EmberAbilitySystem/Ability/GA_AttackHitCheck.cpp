#include "GA_AttackHitCheck.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "EmberLog/EmberLog.h"
#include "GameplayTag/EmberGameplayTag.h"

UGA_AttackHitCheck::UGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentLevel = TriggerEventData->EventMagnitude;
	// Plugin : MeleeAttack * AttackTraceTask = MeleeAttack::CreateTask(this, MeleeAttack::StaticClass());
	// if (AttackTraceTask)
	// {
	// 	AttackTraceTask->OnCompleted.AddDynamic(this, &UGA_AttackHitCheck::OnTraceResultCallback);
	// 	AttackTraceTask->ReadyForActivation();
	// }
	// else
	// {
	// 	EMBER_LOG(LogEmber, Warning, TEXT("Failed to create AttackTraceTask"));
	// }
}

void UGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		AActor* HitActor = HitResult.GetActor();

		UAbilitySystemComponent* SourceAsc = GetAbilitySystemComponentFromActorInfo_Ensured();
		UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (!SourceAsc || !TargetAsc)
		{
			EMBER_LOG(LogEmber, Warning, TEXT("Source or Target ASC is null"));
			return;
		}
		const UEmberCharacterAttributeSet* SourceAttribute = SourceAsc->GetSet<UEmberCharacterAttributeSet>();
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffectClass, CurrentLevel);
		
		if (EffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddHitResult(HitResult);
			FGameplayCueParameters CueParameters;
			CueParameters.EffectContext = CueContextHandle;

			//TargetAsc->ExecuteGameplayCue(EmberGameplayTag::GameplayCue_Character_AttackHit, CueParameters);
		}

		FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffectClass);
		if (BuffEffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle,CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
		}
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
