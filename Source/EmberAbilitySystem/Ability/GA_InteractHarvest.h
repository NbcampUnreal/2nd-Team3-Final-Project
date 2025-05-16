#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_InteractHarvest.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class EMBERABILITYSYSTEM_API UGA_InteractHarvest : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_InteractHarvest();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootHarvest")
	UAnimMontage* Montage;

protected:
	/**
	* ActivateAbility은 "Triggers"에 등록된 Gameplay Event Tag로만 호출
	* TriggerEventData->OptionalObject에 HarvestTarget을 받아와 몽타주를 재생
	* 완료시 Target 액터 삭제
	 */
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> EffectToApply;
	//에디터 실행해서 이팩트 만들고 어트리뷰트셋 포만도 변경 -> 베이스애니멀에 어트리뷰트벨류체인지드포만도변경되면에 포만도 항목으로 함수 바인딩해서 블랙보드 생신
	
private:
	UFUNCTION()
	void OnCompleteCallback();
	
	UFUNCTION()
	void OnInterruptedCallback();
	
	TWeakObjectPtr<const AActor> Instigator;
	TWeakObjectPtr<const AActor> HarvestTarget;
};
