#include "AnimNotify_SwordAttackHitCheck.h"
#include "AbilitySystemBlueprintLibrary.h"

UAnimNotify_SwordAttackHitCheck::UAnimNotify_SwordAttackHitCheck()
{
	ComboAttackLevel = 1.0f;
}

FString UAnimNotify_SwordAttackHitCheck::GetNotifyName_Implementation() const
{
	return TEXT("SwordAttackHitCheck");
}

void UAnimNotify_SwordAttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp)
	{
		if (AActor* Owner = MeshComp->GetOwner())
		{
			FGameplayEventData PayloadData;
			PayloadData.EventMagnitude = ComboAttackLevel; // 여기서 설정한 값은 UABGA_AttackHitCheck의 ActivateAbility 인자값인 TriggerEventData로 전달된다.
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, TriggerGameplayTag,PayloadData);
		}
	}
}
