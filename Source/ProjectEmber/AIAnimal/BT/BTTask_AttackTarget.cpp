// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_AttackTarget.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

UBTTask_AttackTarget::UBTTask_AttackTarget()
{
	NodeName = TEXT("AttackTarget");
}

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController =  OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp =	AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	//공격어빌리티 함수 호출
	ABaseAIAnimal* AICharacter = Cast<ABaseAIAnimal>(AIPawn);
	if (!AICharacter)
	{
		return EBTNodeResult::Failed;
	}
	
	//
	// UObject* TargetObject = BlackboardComp->GetValueAsObject("TargetObject");
	// AActor* TargetActor = Cast<AActor>(TargetObject);
	//
	// FGameplayEventData Payload;
	// Payload.EventTag = FGameplayTag::RequestGameplayTag("Trigger.Animal.Attack");
	// Payload.Instigator = AICharacter;
	// Payload.Target = TargetActor;
	// Payload.OptionalObject = TargetActor;
	// Payload.OptionalObject2 = AICharacter->GetMontage(FGameplayTag::RequestGameplayTag("Animal.Montage.Attack")); //키로 태그 넘겨주면 몽타주 가져옴 -> TSet이나 TMap 으로 바꿀 것  
	//
	// // 게임플레이 이벤트를 어빌리티에게 전달(trigger)하는 함수, 어빌리티가 특정 GameplayTag 이벤트를 수신해서 발동되도록 하는 트리거 역할
	// AICharacter->GetAbilitySystemComponent()->HandleGameplayEvent(Payload.EventTag, &Payload);
	
	// 자기 동물 태그랑 상대 태그랑 같으면 공격 취소

	ABaseAIAnimal* AIAnimal = Cast<ABaseAIAnimal>(AIPawn);
	if (AIAnimal)
	{
		FGameplayTag  IdentityTag = AIAnimal->GetIdentityTag();
		
		UObject* TargetObject = BlackboardComp->GetValueAsObject("TargetObject");
		if (ABaseAIAnimal* TargetActor = Cast<ABaseAIAnimal>(TargetObject))//가져왔는데 동물이면
		{
			FGameplayTag  TargetIdentityTag = TargetActor->GetIdentityTag();
			if (IdentityTag == TargetIdentityTag)
			{
				return EBTNodeResult::Failed;
			}
		}

		//아직 어빌리티가 실행중이면 어택노드가 진행중이라고 처리 -> 노드 들어올때마다 몽타주 재생 방지
		if (AICharacter->GetAbilitySystemComponent()->IsActive())
		{
			return EBTNodeResult::Failed;
		}
		
		FGameplayEventData Payload;
		Payload.EventTag = FGameplayTag::RequestGameplayTag("Trigger.Animal.Attack");
		Payload.Instigator = AICharacter;
		Payload.OptionalObject = AICharacter->GetMontage(FGameplayTag::RequestGameplayTag("Animal.Montage.Attack"));
		AICharacter->GetAbilitySystemComponent()->HandleGameplayEvent(Payload.EventTag, &Payload);
		
		return Super::ExecuteTask(OwnerComp, NodeMemory);

	}
	return EBTNodeResult::Failed;
}

//어빌리티 사용 이유 -> 몽타주 재생중인지 완료인지 판단할 수 있음